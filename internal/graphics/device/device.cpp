#include <cassert>
#include <iostream>

#include "context.hpp"
#include "physical_device.hpp"

#include "surface.hpp"

#include "memory/buffer.hpp"
#include "memory/image.hpp"
// #include "asset/shader.hpp"
#include "asset/pipeline.hpp"
#include "asset/render_pass.hpp"
#include "asset/shader.hpp"
#include "framebuffer.hpp"
#include "swapchain.hpp"

#include "device.hpp"

LogicalDevice::LogicalDevice(const LogicalDeviceCreateInfoT createInfo)
    : cx(createInfo.context), physicalHandle(createInfo.physicalHandle)
{
    VkResult res = cx->CreateDevice(physicalHandle->getHandle(),
                                    (VkDeviceCreateInfo*)createInfo.createInfo, nullptr, &m_handle);
    if (res != VK_SUCCESS)
    {
        std::cerr << "Failed to create logical device : " << res << std::endl;
        return;
    }

    cx->loadBottom(this);

    retrieveQueues();
    createCommandPools();
}

LogicalDevice::~LogicalDevice()
{
#ifdef ENABLE_VIDEO_TRANSCODE
    if (commandPoolEncode)
        cx->DestroyCommandPool(m_handle, commandPoolEncode, nullptr);
    if (commandPoolDecode)
        cx->DestroyCommandPool(m_handle, commandPoolDecode, nullptr);
#endif

    cx->DestroyCommandPool(m_handle, commandPoolTransient, nullptr);
    cx->DestroyCommandPool(m_handle, commandPool, nullptr);

    cx->DestroyDevice(m_handle, nullptr);
}

std::unique_ptr<SwapChain> LogicalDevice::createSwapChain(SwapChainCreateInfoT ci) const
{
    VkSurfaceCapabilitiesKHR capabilities = physicalHandle->getSurfaceCapabilities(*ci.surface);
    auto compatibility = physicalHandle->getSurfaceDetails(*ci.surface);
    auto surfaceFormat = compatibility.formats[0];
    auto presentMode = compatibility.presentModes[0];
    int temp;
    auto surfaceFormatOpt =
        compatibility.findFormat(ci.surfaceFormat.format, ci.surfaceFormat.colorSpace, temp);
    if (surfaceFormatOpt.has_value())
        surfaceFormat = surfaceFormatOpt.value();
    auto presentModeOpt = compatibility.findPresentMode(ci.presentMode, temp);
    if (presentModeOpt.has_value())
        presentMode = presentModeOpt.value();
    auto extent = compatibility.findExtent(ci.extent.width, ci.extent.height);

    uint32_t imageCount = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 && capabilities.maxImageCount < imageCount)
        imageCount = capabilities.maxImageCount;

    VkSwapchainCreateInfoKHR createInfo = {.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
                                           .surface = ci.surface->getHandle(),
                                           .minImageCount = imageCount,
                                           .imageFormat = surfaceFormat.format,
                                           .imageColorSpace = surfaceFormat.colorSpace,
                                           .imageExtent = extent,
                                           .imageArrayLayers = 1,
                                           .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                                           .preTransform = capabilities.currentTransform,
                                           .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
                                           .presentMode = presentMode,
                                           .clipped = VK_TRUE,
                                           .oldSwapchain = VK_NULL_HANDLE};

    std::vector<uint32_t> queueFamilyIndices;
    if (physicalHandle->getGraphicsFamilyIndex().has_value())
        queueFamilyIndices.emplace_back(physicalHandle->getGraphicsFamilyIndex().value());
    if (physicalHandle->getPresentFamilyIndex().has_value())
        queueFamilyIndices.emplace_back(physicalHandle->getPresentFamilyIndex().value());

    if ((physicalHandle->getGraphicsFamilyIndex().has_value() &&
         physicalHandle->getPresentFamilyIndex().has_value()) &&
        physicalHandle->getGraphicsFamilyIndex().value() !=
            physicalHandle->getPresentFamilyIndex().value())
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices.data();
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }

    ci.device = std::make_optional<const LogicalDevice*>(this);
    std::unique_ptr<SwapChain> out = std::make_unique<SwapChain>(ci);
    VkResult res = cx->CreateSwapchainKHR(m_handle, &createInfo, nullptr, &out->getHandle());
    if (res != VK_SUCCESS)
    {
        std::cerr << "Failed to create swapchain : " << res << std::endl;
        return nullptr;
    }

    // prepare the swapchain object
    cx->GetSwapchainImagesKHR(m_handle, out->getHandle(), &imageCount, nullptr);
    out->images.resize(imageCount);
    cx->GetSwapchainImagesKHR(m_handle, out->getHandle(), &imageCount, out->images.data());

    out->imageViews.reserve(imageCount);
    for (int i = 0; i < imageCount; ++i)
    {
        if (!ci.viewCreateInfo.image.has_value())
            ci.viewCreateInfo.image = out->images[i];
        if (!ci.viewCreateInfo.format.has_value())
            ci.viewCreateInfo.format = surfaceFormat.format;
        out->imageViews.emplace_back(createImageView(ci.viewCreateInfo));
    }

    out->imageFormat = surfaceFormat.format;
    out->imageExtent = extent;

    return std::move(out);
}
void LogicalDevice::destroySwapChain(SwapChain& sc) const
{
    for (auto& imageView : sc.imageViews)
    {
        cx->DestroyImageView(m_handle, imageView->handle, nullptr);
    }
    cx->DestroySwapchainKHR(m_handle, sc.getHandle(), nullptr);
}

std::shared_ptr<ImageView> LogicalDevice::createImageView(const ImageViewCreateInfoT ci) const
{
    assert(ci.image.has_value());
    assert(ci.format.has_value());
    VkImageViewCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = ci.image.value(),
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = ci.format.value(),
        .components =
            {
                         .r = VK_COMPONENT_SWIZZLE_R,
                         .g = VK_COMPONENT_SWIZZLE_G,
                         .b = VK_COMPONENT_SWIZZLE_B,
                         .a = VK_COMPONENT_SWIZZLE_A,
                         },
        .subresourceRange =
            {
                         .aspectMask = ci.aspect,
                         .baseMipLevel = 0,
                         .levelCount = 1,
                         .baseArrayLayer = 0,
                         .layerCount = 1,
                         },
    };

    VkImageView imageView;
    VkResult res = cx->CreateImageView(m_handle, &createInfo, nullptr, &imageView);
    if (res != VK_SUCCESS)
        std::cerr << "Failed to create image view : " << res << std::endl;

    return std::make_shared<ImageView>(imageView);
}
void LogicalDevice::destroyImageView(std::shared_ptr<ImageView>& pData) const
{
}

std::shared_ptr<RenderPass> LogicalDevice::createRenderPass(const RenderPassCreateInfoT ci) const
{
    std::vector<VkAttachmentDescription> attachments(ci.colorAttachments.size() + 1);
    std::vector<VkAttachmentReference> colorAttachmentRefs(ci.colorAttachments.size());
    for (int i = 0; i < ci.colorAttachments.size(); ++i)
    {
        auto& r = ci.colorAttachments[i];

        attachments[i] = r.first;
        colorAttachmentRefs[i] = r.second;
    }
    attachments[ci.colorAttachments.size()] = ci.depthAttachment.first;

    std::vector<VkSubpassDescription> subpasses(ci.subpasses.size());
    // there is one set of color attachments per subpasses, hence the vector of vector
    std::vector<std::vector<VkAttachmentReference>> subpassColorAttachments(ci.subpasses.size());
    for (int i = 0; i < ci.subpasses.size(); ++i)
    {
        auto& s = ci.subpasses[i];

        subpassColorAttachments[i] =
            std::vector<VkAttachmentReference>(s.colorAttachmentIndices.size());
        for (int j = 0; j < s.colorAttachmentIndices.size(); ++i)
        {
            subpassColorAttachments[i][j] = ci.colorAttachments[s.colorAttachmentIndices[j]].second;
        }
        subpasses[i] = {
            .pipelineBindPoint = s.pipelineBindPoint,
            .colorAttachmentCount = static_cast<uint32_t>(s.colorAttachmentIndices.size()),
            .pColorAttachments = subpassColorAttachments[i].data(),
        };
        if (s.bDepthAttachment)
            subpasses[i].pDepthStencilAttachment = &ci.depthAttachment.second;
    }
    VkRenderPassCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = static_cast<uint32_t>(attachments.size()),
        .pAttachments = attachments.data(),
        .subpassCount = static_cast<uint32_t>(subpasses.size()),
        .pSubpasses = subpasses.data(),
        .dependencyCount = static_cast<uint32_t>(ci.dependencies.size()),
        .pDependencies = ci.dependencies.data(),
    };

    VkRenderPass renderPass;
    VkResult res = cx->CreateRenderPass(m_handle, &createInfo, nullptr, &renderPass);
    if (res != VK_SUCCESS)
        std::cerr << "Failed to create render pass : " << res << std::endl;

    return std::make_shared<RenderPass>(renderPass);
}
void LogicalDevice::destroyRenderPass(std::shared_ptr<RenderPass>& pData) const
{
}

std::shared_ptr<Framebuffer> LogicalDevice::createFramebuffer(const FramebufferCreateInfoT ci) const
{
    std::vector<VkImageView> attachments(ci.attachments.size());
    for (int i = 0; i < ci.attachments.size(); ++i)
    {
        attachments[i] = ci.attachments[i].handle;
    }
    VkFramebufferCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .renderPass = ci.renderPass->handle,
        .attachmentCount = static_cast<uint32_t>(attachments.size()),
        .pAttachments = attachments.data(),
        .width = ci.width,
        .height = ci.height,
        .layers = 1,
    };

    VkFramebuffer fbo;
    VkResult res = cx->CreateFramebuffer(m_handle, &createInfo, nullptr, &fbo);
    if (res != VK_SUCCESS)
        std::cerr << "Failed to create framebuffer : " << res << std::endl;

    return std::make_shared<Framebuffer>(fbo);
}
void LogicalDevice::destroyFramebuffer(std::shared_ptr<Framebuffer>& pData) const
{
}

std::shared_ptr<GPUShader> LogicalDevice::createShader(const ShaderCreateInfoT ci) const
{
    VkShaderModuleCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = ci.source.size(),
        .pCode = reinterpret_cast<const uint32_t*>(ci.source.data()),
    };

    std::shared_ptr<GPUShader> out = std::make_shared<GPUShader>(ci);
    VkResult res = cx->CreateShaderModule(m_handle, &createInfo, nullptr, &out->module);
    if (res != VK_SUCCESS)
        std::cerr << "Failed to create shader module : " << res << std::endl;

    return out;
}
void LogicalDevice::destroyShader(std::shared_ptr<GPUShader>& pData) const
{
    cx->DestroyShaderModule(m_handle, pData->module, nullptr);
}

std::shared_ptr<Pipeline> LogicalDevice::createPipeline(const PipelineCreateInfoT ci) const
{
    std::vector<VkPipelineShaderStageCreateInfo> shaderStagesCreateInfos(ci.shaderStages.size());
    for (int i = 0; i < ci.shaderStages.size(); ++i)
    {
        shaderStagesCreateInfos[i] = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = ci.shaderStages[i]->ci.stage,
            .module = ci.shaderStages[i]->module,
            .pName = ci.shaderStages[i]->ci.entryPoint,
        };
    }

    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = static_cast<uint32_t>(ci.dynamicStates.size()),
        .pDynamicStates = ci.dynamicStates.data(),
    };

    // vertex (enabling the binding for the Vertex structure)
    VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = static_cast<uint32_t>(ci.vertexBindings.size()),
        .pVertexBindingDescriptions = ci.vertexBindings.data(),
        .vertexAttributeDescriptionCount = static_cast<uint32_t>(ci.vertexAttributes.size()),
        .pVertexAttributeDescriptions = ci.vertexAttributes.data(),
    };

    // draw mode
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = ci.topology,
        .primitiveRestartEnable = ci.bPrimitiveRestartEnable,
    };

    // viewport
    VkViewport viewport = {
        .x = 0.f,
        .y = 0.f,
        .width = static_cast<float>(ci.viewportWidth),
        .height = static_cast<float>(ci.viewportHeight),
        .minDepth = 0.f,
        .maxDepth = 1.f,
    };

    VkRect2D scissor = {
        .offset = {0, 0},
        .extent =
            VkExtent2D{
                   .width = ci.viewportWidth,
                   .height = ci.viewportHeight,
                   },
    };

    VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .scissorCount = 1,
    };

    // rasterizer
    VkPipelineRasterizationStateCreateInfo rasterizerCreateInfo = ci.rasterizerCreateInfo;

    // multisampling, anti-aliasing
    VkPipelineMultisampleStateCreateInfo multisamplingCreateInfo = ci.multisamplingCreateInfo;

    VkPipelineDepthStencilStateCreateInfo depthStencilCreateInfo = ci.depthStencilCreateInfo;

    // color blending
    VkPipelineColorBlendStateCreateInfo colorBlendCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_COPY,
        .attachmentCount = static_cast<uint32_t>(ci.colorBlendAttachment.size()),
        .pAttachments = ci.colorBlendAttachment.data(),
        .blendConstants = {ci.blendConstants[0], ci.blendConstants[1], ci.blendConstants[2],
                           ci.blendConstants[3]},
    };

    // pipeline layout
    std::vector<VkDescriptorSetLayout> setLayouts(ci.setLayoutBindings.size());
    for (int i = 0; i < ci.setLayoutBindings.size(); ++i)
    {
        VkDescriptorSetLayoutCreateInfo createInfo = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .bindingCount = static_cast<uint32_t>(ci.setLayoutBindings[i].size()),
            .pBindings = ci.setLayoutBindings[i].data(),
        };

        VkResult res = vkCreateDescriptorSetLayout(m_handle, &createInfo, nullptr, &setLayouts[i]);
        if (res != VK_SUCCESS)
            std::cerr << "Failed to create descriptor set layout : " << res << std::endl;
    }

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = static_cast<uint32_t>(setLayouts.size()),
        .pSetLayouts = setLayouts.data(),
        .pushConstantRangeCount = static_cast<uint32_t>(ci.pushConstantRanges.size()),
        .pPushConstantRanges =
            ci.pushConstantRanges.empty() ? nullptr : ci.pushConstantRanges.data(),
    };

    auto out = std::make_shared<Pipeline>();

    VkResult res = vkCreatePipelineLayout(m_handle, &pipelineLayoutCreateInfo, nullptr,
                                          &out->getLayoutHandle());
    if (res != VK_SUCCESS)
        std::cerr << "Failed to create pipeline layout : " << res << std::endl;

    VkGraphicsPipelineCreateInfo pipelineCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        // shader stage
        .stageCount = static_cast<uint32_t>(shaderStagesCreateInfos.size()),
        .pStages = shaderStagesCreateInfos.data(),
        // fixed function stage
        .pVertexInputState = &vertexInputCreateInfo,
        .pInputAssemblyState = &inputAssemblyCreateInfo,
        .pViewportState = &viewportStateCreateInfo,
        .pRasterizationState = &rasterizerCreateInfo,
        .pMultisampleState = &multisamplingCreateInfo,
        .pDepthStencilState = &depthStencilCreateInfo,
        .pColorBlendState = &colorBlendCreateInfo,
        .pDynamicState = &dynamicStateCreateInfo,
        // pipeline layout
        .layout = layout,
        // render pass
        .renderPass = ci.renderPass.handle,
        .subpass = ci.subpassIndex,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = -1,
    };

    res = vkCreateGraphicsPipelines(m_handle, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr,
                                    &out->getHandle());
    if (res != VK_SUCCESS)
        std::cerr << "Failed to create graphics pipeline : " << res << std::endl;

    return out;
}
void LogicalDevice::destroyPipeline(std::shared_ptr<Pipeline>& pData) const
{
    vkDestroyPipeline(m_handle, pData->getHandle(), nullptr);
    vkDestroyPipelineLayout(m_handle, pData0->getLayoutHandle(), nullptr);
}

std::shared_ptr<BackBufferT> LogicalDevice::createBackBuffer() const
{
    return std::shared_ptr<BackBufferT>();
}

void LogicalDevice::destroyBackBuffer(std::shared_ptr<BackBufferT>& pData) const
{
}

std::shared_ptr<Buffer> LogicalDevice::createBuffer(const BufferCreateInfoT createInfo) const
{
    // auto bufferCreateInfo = (VkBufferCreateInfo*)createInfo;
    // assert(bufferCreateInfo && bufferCreateInfo->sType == VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO);
    // std::shared_ptr<Buffer> out = std::make_shared<Buffer>();
    // cx->CreateBuffer(handle, (VkBufferCreateInfo*)createInfo, nullptr, &out->handle);
    // // TODO : memory allocation
    // return out;
    return nullptr;
}
void LogicalDevice::destroyBuffer(std::shared_ptr<Buffer>& pData) const
{
    // cx->DestroyBuffer(handle, pData->handle, nullptr);
}

void LogicalDevice::retrieveQueues()
{
    auto graphicsFamilyIndex = physicalHandle->getGraphicsFamilyIndex();
    auto presentFamilyIndex = physicalHandle->getPresentFamilyIndex();
#ifdef ENABLE_VIDEO_TRANSCODE
    auto decodeFamilyIndex = physicalHandle->getDecodeFamilyIndex();
    auto encodeFamilyIndex = physicalHandle->getEncodeFamilyIndex();
#endif

    if (graphicsFamilyIndex.has_value())
        cx->GetDeviceQueue(m_handle, graphicsFamilyIndex.value(), 0, &graphicsQueue);
    if (presentFamilyIndex.has_value())
        cx->GetDeviceQueue(m_handle, presentFamilyIndex.value(), 0, &presentQueue);
#ifdef ENABLE_VIDEO_TRANSCODE
    if (decodeFamilyIndex.has_value())
        cx->GetDeviceQueue(m_handle, decodeFamilyIndex.value(), 0, &decodeQueue);
    if (encodeFamilyIndex.has_value())
        cx->GetDeviceQueue(m_handle, encodeFamilyIndex.value(), 0, &encodeQueue);
#endif
}

void LogicalDevice::createCommandPools()
{
    VkCommandPoolCreateInfo resetCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = physicalHandle->findQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT).value(),
    };

    if (graphicsQueue)
    {
        VkResult res = cx->CreateCommandPool(m_handle, &resetCreateInfo, nullptr, &commandPool);
        if (res != VK_SUCCESS)
        {
            std::cerr << "Failed to create reset command pool : " << res << std::endl;
            return;
        }
        VkCommandPoolCreateInfo transientCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
            .queueFamilyIndex = physicalHandle->findQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT).value(),
        };
        res = cx->CreateCommandPool(m_handle, &transientCreateInfo, nullptr, &commandPoolTransient);
        if (res != VK_SUCCESS)
        {
            std::cerr << "Failed to create transient command pool : " << res << std::endl;
            return;
        }
    }

#ifdef ENABLE_VIDEO_TRANSCODE
    if (decodeQueue)
    {
        cx->CommandPoolCreateInfo decodeCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .queueFamilyIndex =
                physicalHandle->findQueueFamilyIndex(VK_QUEUE_VIDEO_DECODE_BIT_KHR).value(),
        };
        res = cx->CreateCommandPool(m_handle, &resetCreateInfo, nullptr, &commandPoolDecode);
        if (res != VK_SUCCESS)
        {
            std::cerr << "Failed to create reset command pool : " << res << std::endl;
            return;
        }
    }

    if (encodeQueue)
    {
        cx->CommandPoolCreateInfo encodeCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .queueFamilyIndex =
                physicalHandle->findQueueFamilyIndex(VK_QUEUE_VIDEO_ENCODE_BIT_KHR).value(),
        };
        res = cx->CreateCommandPool(m_handle, &resetCreateInfo, nullptr, &commandPoolEncode);
        if (res != VK_SUCCESS)
        {
            std::cerr << "Failed to create reset command pool : " << res << std::endl;
            return;
        }
    }
#endif
}
