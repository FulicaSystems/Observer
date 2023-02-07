#include <limits>

#include "mathematics.hpp"
#include "format.hpp"
#include "lowrenderer.hpp"

#include "graphicsdevice.hpp"
#include "vertex.hpp"

#include "graphicspipeline.hpp"

GraphicsPipeline::GraphicsPipeline(LogicalDevice& device)
	: device(device)
{
}

void GraphicsPipeline::create()
{
	// swapchain
	vulkanSwapchain();
	vulkanImageViews();

	// pipeline
	vulkanRenderPass();
	vulkanGraphicsPipeline();

	// framebuffer
	vulkanFramebuffers();
	vulkanCommandPool();
	vulkanCommandBuffer();

	// multithreading
	vulkanMultithreadObjects();
}

void GraphicsPipeline::destroy()
{
	VkDevice ldevice = device.getVkLDevice();
	vkDeviceWaitIdle(ldevice);

	vkDestroySemaphore(ldevice, renderReadySemaphore, nullptr);
	vkDestroySemaphore(ldevice, renderDoneSemaphore, nullptr);
	vkDestroyFence(ldevice, renderOnceFence, nullptr);
	vkDestroyCommandPool(ldevice, commandPool, nullptr);
	for (VkFramebuffer& framebuffer : swapchainFramebuffers)
	{
		vkDestroyFramebuffer(ldevice, framebuffer, nullptr);
	}
	vkDestroyPipeline(ldevice, graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(ldevice, pipelineLayout, nullptr);
	vkDestroyRenderPass(ldevice, renderPass, nullptr);
	for (VkImageView& imageView : swapchainImageViews)
	{
		vkDestroyImageView(ldevice, imageView, nullptr);
	}
	vkDestroySwapchainKHR(ldevice, swapchain, nullptr);
}

VkSurfaceFormatKHR GraphicsPipeline::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	for (const VkSurfaceFormatKHR& availableFormat : availableFormats)
	{
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			return availableFormat;
	}

	return availableFormats[0];
}

VkPresentModeKHR GraphicsPipeline::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availableModes)
{
	for (const VkPresentModeKHR& availableMode : availableModes)
	{
		if (availableMode == VK_PRESENT_MODE_MAILBOX_KHR)
			return availableMode;
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D GraphicsPipeline::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		return capabilities.currentExtent;
	}
	else
	{
		const int height = Format::framebufferHeight;
		const int width = Format::framebufferWidth;

		VkExtent2D actualExtent = {
			.width = static_cast<uint32_t>(width),
			.height = static_cast<uint32_t>(height)
		};

		actualExtent.width = Math::clamp(actualExtent.width,
			capabilities.minImageExtent.width,
			capabilities.maxImageExtent.width);
		actualExtent.height = Math::clamp(actualExtent.height,
			capabilities.minImageExtent.height,
			capabilities.maxImageExtent.height);

		return actualExtent;
	}
}

void GraphicsPipeline::vulkanSwapchain()
{
	VkDevice ldevice = device.getVkLDevice();
	PhysicalDevice pdevice = device.getPDevice();

	VkSwapchainSupportDetails support = pdevice.querySwapchainSupport(device.low.surface);

	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(support.formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(support.presentModes);
	VkExtent2D extent = chooseSwapExtent(support.capabilities);

	uint32_t imageCount = support.capabilities.minImageCount + 1;
	if (support.capabilities.maxImageCount > 0 && support.capabilities.maxImageCount < imageCount)
		imageCount = support.capabilities.maxImageCount;

	VkSwapchainCreateInfoKHR createInfo = {
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.surface = device.low.surface,
		.minImageCount = imageCount,
		.imageFormat = surfaceFormat.format,
		.imageColorSpace = surfaceFormat.colorSpace,
		.imageExtent = extent,
		.imageArrayLayers = 1,
		.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		.preTransform = support.capabilities.currentTransform,
		.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		.presentMode = presentMode,
		.clipped = VK_TRUE,
		.oldSwapchain = VK_NULL_HANDLE
	};

	VkQueueFamilyIndices indices = pdevice.findQueueFamilies(device.low.surface);
	uint32_t queueFamilyIndices[] = {
		indices.graphicsFamily.value(),
		indices.presentFamily.value()
	};

	if (indices.graphicsFamily != indices.presentFamily)
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
	}

	if (vkCreateSwapchainKHR(ldevice, &createInfo, nullptr, &swapchain) != VK_SUCCESS)
		throw std::exception("Failed to create swapchain");

	vkGetSwapchainImagesKHR(ldevice, swapchain, &imageCount, nullptr);
	swapchainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(ldevice, swapchain, &imageCount, swapchainImages.data());

	swapchainImageFormat = surfaceFormat.format;
	swapchainExtent = extent;
}

void GraphicsPipeline::vulkanImageViews()
{
	swapchainImageViews.resize(swapchainImages.size());

	for (size_t i = 0; i < swapchainImages.size(); ++i)
	{
		VkImageViewCreateInfo createInfo = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.image = swapchainImages[i],
			.viewType = VK_IMAGE_VIEW_TYPE_2D,
			.format = swapchainImageFormat,
			.components = {
				.r = VK_COMPONENT_SWIZZLE_IDENTITY,
				.g = VK_COMPONENT_SWIZZLE_IDENTITY,
				.b = VK_COMPONENT_SWIZZLE_IDENTITY,
				.a = VK_COMPONENT_SWIZZLE_IDENTITY
			},
			.subresourceRange = {
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1
			}
		};

		VkDevice ldevice = device.getVkLDevice();
		if (vkCreateImageView(ldevice, &createInfo, nullptr, &swapchainImageViews[i]) != VK_SUCCESS)
			throw std::exception("Failed to create an image view");
	}
}

void GraphicsPipeline::vulkanGraphicsPipeline()
{
	std::vector<char> vs = readBinaryFile("shaders/vstriangle.spv");
	std::vector<char> fs = readBinaryFile("shaders/fstriangle.spv");

	VkShaderModule vsModule = createShaderModule(vs);
	VkShaderModule fsModule = createShaderModule(fs);

	VkPipelineShaderStageCreateInfo vsStageCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		.stage = VK_SHADER_STAGE_VERTEX_BIT,
		.module = vsModule,
		.pName = "main",
		//for shader constants values
		.pSpecializationInfo = nullptr
	};

	VkPipelineShaderStageCreateInfo fsStageCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
		.module = fsModule,
		.pName = "main",
		.pSpecializationInfo = nullptr
	};

	VkPipelineShaderStageCreateInfo shaderStagesCreateInfo[] = {
		vsStageCreateInfo,
		fsStageCreateInfo
	};

	std::vector<VkDynamicState> dynamicStates = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
		.pDynamicStates = dynamicStates.data()
	};

	//vertex buffer (enabling the binding for our Vertex structure)
	auto binding = Vertex::getBindingDescription();
	auto attribs = Vertex::getAttributeDescription();
	VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		.vertexBindingDescriptionCount = 1,
		.pVertexBindingDescriptions = &binding,
		.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribs.size()),
		.pVertexAttributeDescriptions = attribs.data()
	};

	//draw mode
	VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		.primitiveRestartEnable = VK_FALSE
	};

	//viewport
	VkViewport viewport = {
		.x = 0.f,
		.y = 0.f,
		.width = static_cast<float>(swapchainExtent.width),
		.height = static_cast<float>(swapchainExtent.height),
		.minDepth = 0.f,
		.maxDepth = 1.f
	};

	VkRect2D scissor = {
		.offset = { 0, 0 },
		.extent = swapchainExtent
	};

	VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		.viewportCount = 1,
		.scissorCount = 1
	};

	//rasterizer
	VkPipelineRasterizationStateCreateInfo rasterizerCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		.depthClampEnable = VK_FALSE,
		.rasterizerDiscardEnable = VK_FALSE,
		.polygonMode = VK_POLYGON_MODE_FILL,
		.cullMode = VK_CULL_MODE_BACK_BIT,
		.frontFace = VK_FRONT_FACE_CLOCKWISE,
		.depthBiasEnable = VK_FALSE,
		.depthBiasConstantFactor = 0.f,
		.depthBiasClamp = 0.f,
		.depthBiasSlopeFactor = 0.f,
		.lineWidth = 1.f
	};

	//multisampling, anti-aliasing
	VkPipelineMultisampleStateCreateInfo multisamplingCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
		.sampleShadingEnable = VK_FALSE,
		.minSampleShading = 1.f,
		.pSampleMask = nullptr,
		.alphaToCoverageEnable = VK_FALSE,
		.alphaToOneEnable = VK_FALSE
	};

	//color blending
	VkPipelineColorBlendAttachmentState colorBlendAttachment = {
		.blendEnable = VK_TRUE,
		.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
		.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
		.colorBlendOp = VK_BLEND_OP_ADD,
		.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
		.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
		.alphaBlendOp = VK_BLEND_OP_ADD,
		.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
	};

	VkPipelineColorBlendStateCreateInfo colorBlendCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		.logicOpEnable = VK_FALSE,
		.logicOp = VK_LOGIC_OP_COPY,
		.attachmentCount = 1,
		.pAttachments = &colorBlendAttachment,
		.blendConstants = { 0.f, 0.f, 0.f, 0.f }
	};

	//uniforms
	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.setLayoutCount = 0,
		.pSetLayouts = nullptr,
		.pushConstantRangeCount = 0,
		.pPushConstantRanges = nullptr
	};

	VkDevice ldevice = device.getVkLDevice();

	if (vkCreatePipelineLayout(ldevice, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
		throw std::exception("Failed to create pipeline layout");

	VkGraphicsPipelineCreateInfo pipelineCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		//shader stage
		.stageCount = 2,
		.pStages = shaderStagesCreateInfo,
		//fixed function stage
		.pVertexInputState = &vertexInputCreateInfo,
		.pInputAssemblyState = &inputAssemblyCreateInfo,
		.pViewportState = &viewportStateCreateInfo,
		.pRasterizationState = &rasterizerCreateInfo,
		.pMultisampleState = &multisamplingCreateInfo,
		.pDepthStencilState = nullptr,
		.pColorBlendState = &colorBlendCreateInfo,
		.pDynamicState = &dynamicStateCreateInfo,
		//pipeline layout
		.layout = pipelineLayout,
		//render pass
		.renderPass = renderPass,
		.subpass = 0,
		.basePipelineHandle = VK_NULL_HANDLE,
		.basePipelineIndex = -1
	};

	if (vkCreateGraphicsPipelines(ldevice, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &graphicsPipeline) != VK_SUCCESS)
		throw std::exception("Failed to create graphics pipeline");

	vkDestroyShaderModule(ldevice, vsModule, nullptr);
	vkDestroyShaderModule(ldevice, fsModule, nullptr);
}

VkShaderModule GraphicsPipeline::createShaderModule(const std::vector<char>& code)
{
	VkShaderModuleCreateInfo createInfo = {
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.codeSize = code.size(),
		.pCode = reinterpret_cast<const uint32_t*>(code.data())
	};

	VkDevice ldevice = device.getVkLDevice();
	VkShaderModule shaderModule;
	if (vkCreateShaderModule(ldevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
		throw std::exception("Failed to create shader module");
	return shaderModule;
}

void GraphicsPipeline::vulkanRenderPass()
{
	VkAttachmentDescription colorAttachment = {
		.format = swapchainImageFormat,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,		//load : what to do with the already existing image on the framebuffer
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,	//store : what to do with the newly rendered image on the framebuffer
		.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
	};

	VkAttachmentReference colorAttachmentRef = {
		.attachment = 0,	//colorAttachment is index 0
		.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	};

	VkSubpassDescription subpass = {
		.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
		.colorAttachmentCount = 1,
		.pColorAttachments = &colorAttachmentRef
	};

	VkSubpassDependency dependency = {
		.srcSubpass = VK_SUBPASS_EXTERNAL,
		.dstSubpass = 0,
		.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		.srcAccessMask = 0,
		.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
	};

	VkRenderPassCreateInfo createInfo = {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.attachmentCount = 1,
		.pAttachments = &colorAttachment,
		.subpassCount = 1,
		.pSubpasses = &subpass,
		.dependencyCount = 1,
		.pDependencies = &dependency
	};

	VkDevice ldevice = device.getVkLDevice();
	if (vkCreateRenderPass(ldevice, &createInfo, nullptr, &renderPass) != VK_SUCCESS)
		throw std::exception("Failed to create render pass");
}

void GraphicsPipeline::vulkanFramebuffers()
{
	VkDevice ldevice = device.getVkLDevice();
	swapchainFramebuffers.resize(swapchainImageViews.size());

	for (size_t i = 0; i < swapchainImageViews.size(); ++i)
	{
		VkFramebufferCreateInfo createInfo = {
			.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			.renderPass = renderPass,
			.attachmentCount = 1,
			.pAttachments = &swapchainImageViews[i],
			.width = swapchainExtent.width,
			.height = swapchainExtent.height,
			.layers = 1
		};

		if (vkCreateFramebuffer(ldevice, &createInfo, nullptr, &swapchainFramebuffers[i]) != VK_SUCCESS)
			throw std::exception("Failed to create framebuffer");
	}
}

void GraphicsPipeline::vulkanCommandPool()
{
	VkDevice ldevice = device.getVkLDevice();
	PhysicalDevice pdevice = device.getPDevice();

	VkQueueFamilyIndices indices = pdevice.findQueueFamilies(device.low.surface);

	VkCommandPoolCreateInfo createInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		.queueFamilyIndex = indices.graphicsFamily.value()
	};

	if (vkCreateCommandPool(ldevice, &createInfo, nullptr, &commandPool) != VK_SUCCESS)
		throw std::exception("Failed to create command pool");
}

void GraphicsPipeline::vulkanCommandBuffer()
{
	VkDevice ldevice = device.getVkLDevice();

	VkCommandBufferAllocateInfo allocInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.commandPool = commandPool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = 1
	};

	if (vkAllocateCommandBuffers(ldevice, &allocInfo, &commandBuffer) != VK_SUCCESS)
		throw std::exception("Failed to allocate command buffers");
}

void GraphicsPipeline::recordCommandBuffer(VkCommandBuffer cb, uint32_t imageIndex)
{
	VkCommandBufferBeginInfo commandBufferBeginInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.flags = 0,
		.pInheritanceInfo = nullptr
	};

	if (vkBeginCommandBuffer(cb, &commandBufferBeginInfo) != VK_SUCCESS)
		throw std::exception("Failed to begin recording command buffer");

	VkClearValue clearColor = {
		.color = { 0.2f, 0.2f, 0.2f, 1.f }
	};

	VkRenderPassBeginInfo renderPassBeginInfo = {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.renderPass = renderPass,
		.framebuffer = swapchainFramebuffers[imageIndex],
		.renderArea = {
			.offset = { 0, 0 },
			.extent = swapchainExtent
			},
		.clearValueCount = 1,
		.pClearValues = &clearColor
	};

	vkCmdBeginRenderPass(cb, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

	VkViewport viewport = {
		.x = 0.f,
		.y = 0.f,
		.width = static_cast<float>(swapchainExtent.width),
		.height = static_cast<float>(swapchainExtent.height),
		.minDepth = 0.f,
		.maxDepth = 1.f
	};

	vkCmdSetViewport(cb, 0, 1, &viewport);

	VkRect2D scissor = {
		.offset = { 0, 0 },
		.extent = swapchainExtent
	};

	vkCmdSetScissor(cb, 0, 1, &scissor);

	vkCmdDraw(cb, 3, 1, 0, 0);

	vkCmdEndRenderPass(cb);

	if (vkEndCommandBuffer(cb) != VK_SUCCESS)
		throw std::exception("Failed to record command buffer");
}

void GraphicsPipeline::drawFrame()
{
	VkDevice ldevice = device.getVkLDevice();

	vkWaitForFences(ldevice, 1, &renderOnceFence, VK_TRUE, UINT64_MAX);
	vkResetFences(ldevice, 1, &renderOnceFence);

	uint32_t imageIndex;
	vkAcquireNextImageKHR(ldevice, swapchain, UINT64_MAX, renderReadySemaphore, VK_NULL_HANDLE, &imageIndex);

	vkResetCommandBuffer(commandBuffer, 0);
	recordCommandBuffer(commandBuffer, imageIndex);

	VkSemaphore waitSemaphores[] = { renderReadySemaphore };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	VkSemaphore signalSemaphores[] = { renderDoneSemaphore };
	VkSubmitInfo submitInfo = {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = waitSemaphores,
		.pWaitDstStageMask = waitStages,
		.commandBufferCount = 1,
		.pCommandBuffers = &commandBuffer,
		.signalSemaphoreCount = 1,
		.pSignalSemaphores = signalSemaphores
	};

	if (vkQueueSubmit(device.graphicsQueue, 1, &submitInfo, renderOnceFence) != VK_SUCCESS)
		throw std::exception("Failed to submit draw command buffer");

	VkSwapchainKHR swapchains[] = { swapchain };
	VkPresentInfoKHR presentInfo = {
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = signalSemaphores,
		.swapchainCount = 1,
		.pSwapchains = swapchains,
		.pImageIndices = &imageIndex,
		.pResults = nullptr
	};

	vkQueuePresentKHR(device.presentQueue, &presentInfo);
}

void GraphicsPipeline::vulkanMultithreadObjects()
{
	VkSemaphoreCreateInfo semaphoreCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
	};

	VkFenceCreateInfo fenceCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		.flags = VK_FENCE_CREATE_SIGNALED_BIT
	};

	VkDevice ldevice = device.getVkLDevice();

	if (vkCreateSemaphore(ldevice, &semaphoreCreateInfo, nullptr, &renderReadySemaphore) != VK_SUCCESS)
		throw std::exception("Failed to create semaphore");
	if (vkCreateSemaphore(ldevice, &semaphoreCreateInfo, nullptr, &renderDoneSemaphore) != VK_SUCCESS)
		throw std::exception("Failed to create semaphore");
	if (vkCreateFence(ldevice, &fenceCreateInfo, nullptr, &renderOnceFence) != VK_SUCCESS)
		throw std::exception("Failed to create fence");
}