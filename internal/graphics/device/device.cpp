#include <cassert>
#include <iostream>

#include "context.hpp"
#include "physical_device.hpp"

#include "surface.hpp"

#include "memory/buffer.hpp"
#include "memory/image.hpp"
// #include "asset/shader.hpp"
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
    VkResult res = vkCreateImageView(m_handle, &createInfo, nullptr, &imageView);
    if (res != VK_SUCCESS)
        std::cerr << "Failed to create image view : " << res << std::endl;

    return std::make_shared<ImageView>(imageView);
}

std::shared_ptr<Buffer> LogicalDevice::createBuffer(const BufferCreateInfoT createInfo) const
{
    // auto bufferCreateInfo = (VkBufferCreateInfo*)createInfo;
    // assert(bufferCreateInfo && bufferCreateInfo->sType == VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO);
    // std::shared_ptr<Buffer> out = std::make_shared<Buffer>();
    // vkCreateBuffer(handle, (VkBufferCreateInfo*)createInfo, nullptr, &out->handle);
    // // TODO : memory allocation
    // return out;
    return nullptr;
}

void LogicalDevice::destroyBuffer(std::shared_ptr<Buffer>& pData) const
{
    // vkDestroyBuffer(handle, pData->handle, nullptr);
}

// std::shared_ptr<ShaderModule> LogicalDevice::createShaderModule(
//     const ShaderModuleCreateInfoT createInfo) const
// {
//     auto shaderModuleCreateInfo = (VkShaderModuleCreateInfo*)createInfo;
//     assert(shaderModuleCreateInfo &&
//            shaderModuleCreateInfo->sType == VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO);
//     std::shared_ptr<ShaderModule> out = std::make_shared<ShaderModule>();
//     vkCreateShaderModule(handle, (VkShaderModuleCreateInfo*)createInfo, nullptr, &out->handle);
//     return out;
// }

// void LogicalDevice::destroyShaderModule(std::shared_ptr<ShaderModule>& pData) const
// {
//     vkDestroyShaderModule(handle, pData->handle, nullptr);
// }

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
