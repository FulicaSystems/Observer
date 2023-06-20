#include <limits>
#include <stdexcept>

#include "mathematics.hpp"
#include "format.hpp"

#include "renderer.hpp"

#include "vertex.hpp"
#include "vertexbuffer_vk.hpp"

#include "resourcesmanager.hpp"
#include "shader.hpp"

#include "lowrenderer_vk.hpp"
#include "graphicsdevice_vk.hpp"
#include "shadermodule_vk.hpp"
#include "graphicspipeline_vk.hpp"

void GraphicsPipeline_Vk::create(ILowRenderer* api, ILogicalDevice* device)
{
	Super::create(api, device);
	shader = ResourcesManager::load<Shader>("triangle_shader", "shaders/triangle", *api->highRenderer);

	// swapchain
	vulkanSwapchain();
	vulkanImageViews();

	// pipeline
	vulkanRenderPass();
	vulkanGraphicsPipeline();

	// framebuffer
	vulkanFramebuffers();

	// multithreading
	vulkanMultithreadObjects();
}

void GraphicsPipeline_Vk::destroy()
{
	const VkDevice& vkdevice = ((LogicalDevice_Vk*)device)->vkdevice;
	vkDeviceWaitIdle(vkdevice);

	vkDestroySemaphore(vkdevice, renderReadySemaphore, nullptr);
	vkDestroySemaphore(vkdevice, renderDoneSemaphore, nullptr);
	vkDestroyFence(vkdevice, renderOnceFence, nullptr);
	for (VkFramebuffer& framebuffer : swapchainFramebuffers)
	{
		vkDestroyFramebuffer(vkdevice, framebuffer, nullptr);
	}
	shader.reset();
	vkDestroyPipeline(vkdevice, graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(vkdevice, pipelineLayout, nullptr);
	vkDestroyRenderPass(vkdevice, renderPass, nullptr);
	for (VkImageView& imageView : swapchainImageViews)
	{
		vkDestroyImageView(vkdevice, imageView, nullptr);
	}
	vkDestroySwapchainKHR(vkdevice, swapchain, nullptr);
}

VkSurfaceFormatKHR GraphicsPipeline_Vk::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	for (const VkSurfaceFormatKHR& availableFormat : availableFormats)
	{
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			return availableFormat;
	}

	return availableFormats[0];
}

VkPresentModeKHR GraphicsPipeline_Vk::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availableModes)
{
	for (const VkPresentModeKHR& availableMode : availableModes)
	{
		if (availableMode == VK_PRESENT_MODE_MAILBOX_KHR)
			return availableMode;
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D GraphicsPipeline_Vk::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
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

void GraphicsPipeline_Vk::vulkanSwapchain()
{
	VkDevice vkdevice = ((LogicalDevice_Vk*)device)->vkdevice;
	PhysicalDevice vkpdevice = ((LogicalDevice_Vk*)device)->pdevice.vkpdevice;

	VkSwapchainSupportDetails support = vkpdevice.querySwapchainSupport(((LowRenderer_Vk*)api)->surface);

	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(support.formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(support.presentModes);
	VkExtent2D extent = chooseSwapExtent(support.capabilities);

	uint32_t imageCount = support.capabilities.minImageCount + 1;
	if (support.capabilities.maxImageCount > 0 && support.capabilities.maxImageCount < imageCount)
		imageCount = support.capabilities.maxImageCount;

	VkSwapchainCreateInfoKHR createInfo = {
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.surface = ((LowRenderer_Vk*)api)->surface,
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

	VkQueueFamilyIndices indices = vkpdevice.findQueueFamilies(((LowRenderer_Vk*)api)->surface);
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

	if (vkCreateSwapchainKHR(vkdevice, &createInfo, nullptr, &swapchain) != VK_SUCCESS)
		throw std::runtime_error("Failed to create swapchain");

	vkGetSwapchainImagesKHR(vkdevice, swapchain, &imageCount, nullptr);
	swapchainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(vkdevice, swapchain, &imageCount, swapchainImages.data());

	swapchainImageFormat = surfaceFormat.format;
	swapchainExtent = extent;
}

void GraphicsPipeline_Vk::vulkanImageViews()
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

		VkDevice vkdevice = ((LogicalDevice_Vk*)device)->vkdevice;
		if (vkCreateImageView(vkdevice, &createInfo, nullptr, &swapchainImageViews[i]) != VK_SUCCESS)
			throw std::runtime_error("Failed to create an image view");
	}
}

void GraphicsPipeline_Vk::vulkanGraphicsPipeline()
{
	auto initPipeline = [=, this]() {
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

		VkDevice vkdevice = ((LogicalDevice_Vk*)device)->vkdevice;

		if (vkCreatePipelineLayout(vkdevice, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
			throw std::runtime_error("Failed to create pipeline layout");

		VkGraphicsPipelineCreateInfo pipelineCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
			//shader stage
			.stageCount = 2,
			.pStages = std::dynamic_pointer_cast<ShaderModule_Vk>(shader->local)->getCreateInfo().data(),
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

		if (vkCreateGraphicsPipelines(vkdevice, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &graphicsPipeline) != VK_SUCCESS)
			throw std::runtime_error("Failed to create graphics pipeline");
	};

	Utils::GlobalThreadPool::addTask([=, this]() {
		// wait for shader loading
		shader->loaded.wait(false);
		Utils::GlobalThreadPool::addTask([=, this]() {
			initPipeline();
			readyToDraw.test_and_set();
			}, false);
		});
}

void GraphicsPipeline_Vk::vulkanRenderPass()
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

	VkDevice vkdevice = ((LogicalDevice_Vk*)device)->vkdevice;
	if (vkCreateRenderPass(vkdevice, &createInfo, nullptr, &renderPass) != VK_SUCCESS)
		throw std::runtime_error("Failed to create render pass");
}

void GraphicsPipeline_Vk::vulkanFramebuffers()
{
	VkDevice vkdevice = ((LogicalDevice_Vk*)device)->vkdevice;
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

		if (vkCreateFramebuffer(vkdevice, &createInfo, nullptr, &swapchainFramebuffers[i]) != VK_SUCCESS)
			throw std::runtime_error("Failed to create framebuffer");
	}
}

void GraphicsPipeline_Vk::recordImageCommandBuffer(CommandBuffer_Vk& cb,
	uint32_t imageIndex,
	const std::deque<std::shared_ptr<IVertexBuffer>>& vbos)
{
	cb.reset();
	cb.beginRecord();

	VkCommandBuffer& cbo = cb.get();

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

	vkCmdBeginRenderPass(cbo, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline(cbo, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

	VkViewport viewport = {
		.x = 0.f,
		.y = 0.f,
		.width = static_cast<float>(swapchainExtent.width),
		.height = static_cast<float>(swapchainExtent.height),
		.minDepth = 0.f,
		.maxDepth = 1.f
	};

	vkCmdSetViewport(cbo, 0, 1, &viewport);

	VkRect2D scissor = {
		.offset = { 0, 0 },
		.extent = swapchainExtent
	};

	vkCmdSetScissor(cbo, 0, 1, &scissor);

	// bind VBOs
	for (int i = 0; i < vbos.size(); ++i)
	{
		const VertexBuffer_Vk& vbo = *(VertexBuffer_Vk*)vbos.at(i).get();

		VkBuffer buffers[] = { vbo.buffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(cbo, 0, 1, buffers, offsets);
		vkCmdDraw(cbo, vbo.vertexNum, 1, 0, 0);
	}

	vkCmdEndRenderPass(cbo);

	cb.endRecord();
}

void GraphicsPipeline_Vk::drawFrame(CommandBuffer_Vk& cb, const std::deque<std::shared_ptr<IVertexBuffer>>& vbos)
{
	if (!readyToDraw.test())
		return;

	VkDevice vkdevice = ((LogicalDevice_Vk*)device)->vkdevice;

	vkWaitForFences(vkdevice, 1, &renderOnceFence, VK_TRUE, UINT64_MAX);
	vkResetFences(vkdevice, 1, &renderOnceFence);

	uint32_t imageIndex;
	vkAcquireNextImageKHR(vkdevice, swapchain, UINT64_MAX, renderReadySemaphore, VK_NULL_HANDLE, &imageIndex);

	recordImageCommandBuffer(cb, imageIndex, vbos);

	VkSemaphore waitSemaphores[] = { renderReadySemaphore };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	VkSemaphore signalSemaphores[] = { renderDoneSemaphore };
	VkSubmitInfo submitInfo = {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = waitSemaphores,
		.pWaitDstStageMask = waitStages,
		.commandBufferCount = 1,
		.pCommandBuffers = &cb.get(),
		.signalSemaphoreCount = 1,
		.pSignalSemaphores = signalSemaphores
	};

	((LogicalDevice_Vk*)device)->submitCommandToGraphicsQueue(submitInfo, renderOnceFence);

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

	((LogicalDevice_Vk*)device)->present(presentInfo);
}

void GraphicsPipeline_Vk::vulkanMultithreadObjects()
{
	VkSemaphoreCreateInfo semaphoreCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
	};

	VkFenceCreateInfo fenceCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		.flags = VK_FENCE_CREATE_SIGNALED_BIT
	};

	VkDevice vkdevice = ((LogicalDevice_Vk*)device)->vkdevice;

	if (vkCreateSemaphore(vkdevice, &semaphoreCreateInfo, nullptr, &renderReadySemaphore) != VK_SUCCESS)
		throw std::runtime_error("Failed to create semaphore");
	if (vkCreateSemaphore(vkdevice, &semaphoreCreateInfo, nullptr, &renderDoneSemaphore) != VK_SUCCESS)
		throw std::runtime_error("Failed to create semaphore");
	if (vkCreateFence(vkdevice, &fenceCreateInfo, nullptr, &renderOnceFence) != VK_SUCCESS)
		throw std::runtime_error("Failed to create fence");
}