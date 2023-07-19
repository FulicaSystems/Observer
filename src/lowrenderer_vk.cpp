#include <iostream>
#include <stdexcept>
#include <set>

#include "format.hpp"
#include "utils/multithread/globalthreadpool.hpp"

#include "commandbuffer_vk.hpp"
#include "lowrenderer_vk.hpp"
#include "resourcesmanager.hpp"

#include "vertex.hpp"
#include "vertexbuffer_vk.hpp"
#include "shader.hpp"
#include "shadermodule_vk.hpp"

bool PhysicalDevice::checkDeviceExtensionSupport()
{
	uint32_t extensionCount = 0;
	vkEnumerateDeviceExtensionProperties(vkpdevice, nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(vkpdevice, nullptr, &extensionCount, extensions.data());
	std::cout << "available device extensions : " << extensionCount << '\n';
	for (const auto& extension : extensions)
		std::cout << '\t' << extension.extensionName << '\n';

	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for (const VkExtensionProperties& extension : extensions)
	{
		//is the required extension available?
		requiredExtensions.erase(extension.extensionName);
	}

	//are all required extensions found in the available extension list?
	return requiredExtensions.empty();
}

PhysicalDevice::PhysicalDevice(VkPhysicalDevice vk)
{
	vkpdevice = vk;
}

VkSwapchainSupportDetails PhysicalDevice::querySwapchainSupport(const VkSurfaceKHR& surface)
{
	VkSwapchainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkpdevice, surface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(vkpdevice, surface, &formatCount, nullptr);
	if (formatCount != 0)
	{
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(vkpdevice, surface, &formatCount, details.formats.data());
	}

	uint32_t modeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(vkpdevice, surface, &modeCount, nullptr);
	if (modeCount != 0)
	{
		details.presentModes.resize(modeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(vkpdevice, surface, &modeCount, details.presentModes.data());
	}

	return details;
}

bool PhysicalDevice::isDeviceSuitable(const VkSurfaceKHR& surface)
{
#if false
	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceProperties(pdevice, &deviceProperties);
	vkGetPhysicalDeviceFeatures(pdevice, &deviceFeatures);

	return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
		deviceFeatures.geometryShader;
#else
	VkQueueFamilyIndices indices = findQueueFamilies(surface);

	bool extensionSupport = checkDeviceExtensionSupport();

	bool swapchainSupport = false;
	if (extensionSupport)
	{
		VkSwapchainSupportDetails support = querySwapchainSupport(surface);
		swapchainSupport = !support.formats.empty() && !support.presentModes.empty();
	}

	return indices.isComplete() && extensionSupport && swapchainSupport;
#endif
}

uint32_t PhysicalDevice::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const
{
	VkPhysicalDeviceMemoryProperties memProp;
	vkGetPhysicalDeviceMemoryProperties(vkpdevice, &memProp);

	for (uint32_t i = 0; i < memProp.memoryTypeCount; ++i)
	{
		bool rightType = typeFilter & (1 << i);
		bool rightFlag = (memProp.memoryTypes[i].propertyFlags & properties) == properties;
		if (rightType && rightFlag)
			return i;
	}

	throw std::runtime_error("Failed to find suitable memory type");
}

VkQueueFamilyIndices PhysicalDevice::findQueueFamilies(const VkSurfaceKHR& surface)
{
	VkQueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(vkpdevice, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(vkpdevice, &queueFamilyCount, queueFamilies.data());
	for (uint32_t i = 0; i < queueFamilies.size(); ++i)
	{
		//graphics family
		if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			indices.graphicsFamily = i;

		//presentation family
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(vkpdevice, i, surface, &presentSupport);
		if (presentSupport)
			indices.presentFamily = i;
	}

	return indices;
}

struct LowRenderer_Vk::LogicalDevice
{
	LowRenderer_Vk* q;

	VkQueue graphicsQueue;
	VkQueue presentQueue;

	PhysicalDevice pdevice;
	VkDevice vkdevice;

	void vulkanPhysicalDevice()
	{
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(q->instance, &deviceCount, nullptr);
		if (deviceCount == 0)
			throw std::runtime_error("Failed to find GPUs with Vulkan support");

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(q->instance, &deviceCount, devices.data());
		std::cout << "available devices : " << deviceCount << '\n';

		for (const auto& device : devices)
		{
			VkPhysicalDeviceProperties properties;
			vkGetPhysicalDeviceProperties(device, &properties);
			std::cout << '\t' << properties.deviceName << '\n';
		}

		//select the last available device (most likely discrete gpu)
		for (const auto& device : devices)
		{
			PhysicalDevice d(device);
			if (d.isDeviceSuitable(q->surface))
			{
				pdevice = device;
				//break;
			}
		}

		VkPhysicalDevice d = pdevice.vkpdevice;
		if (d == VK_NULL_HANDLE)
			throw std::runtime_error("Failed to find a suitable GPU");
		if (!gladLoaderLoadVulkan(q->instance, d, nullptr))
			throw std::runtime_error("Unable to reload Vulkan symbols with physical device");

		// physical device limits
		VkPhysicalDeviceProperties prop;
		vkGetPhysicalDeviceProperties(pdevice.vkpdevice, &prop);
		VkPhysicalDeviceLimits limit = prop.limits;
		std::cout << "Physical device max memory allocation count : " << limit.maxMemoryAllocationCount << std::endl;
	}
	void vulkanLogicalDevice()
	{
		VkQueueFamilyIndices indices = pdevice.findQueueFamilies(q->surface);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

		float queuePriority = 1.f;
		for (uint32_t queueFamily : uniqueQueueFamilies)
		{
			VkDeviceQueueCreateInfo queueCreateInfo = {
				.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
				.queueFamilyIndex = queueFamily,
				.queueCount = 1,
				.pQueuePriorities = &queuePriority
			};
			queueCreateInfos.emplace_back(queueCreateInfo);
		}

		VkDeviceCreateInfo createInfo = {
			.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
			.pQueueCreateInfos = queueCreateInfos.data(),
	#ifdef NDEBUG
			.enabledLayerCount = 0,
	#else
			.enabledLayerCount = static_cast<uint32_t>(validationLayers.size()),
			.ppEnabledLayerNames = validationLayers.data(),
	#endif
			.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()),
			.ppEnabledExtensionNames = deviceExtensions.data()
		};

		VkPhysicalDevice d = pdevice.vkpdevice;

		if (vkCreateDevice(d, &createInfo, nullptr, &vkdevice) != VK_SUCCESS)
			throw std::runtime_error("Failed to create logical device");
		if (!gladLoaderLoadVulkan(q->instance, d, vkdevice))
			throw std::runtime_error("Unable to reload Vulkan symbols with logical device");

		vkGetDeviceQueue(vkdevice, indices.graphicsFamily.value(), 0, &graphicsQueue);
		vkGetDeviceQueue(vkdevice, indices.presentFamily.value(), 0, &presentQueue);
	}

	LogicalDevice(LowRenderer_Vk* q) : q(q)
	{
		vulkanPhysicalDevice();
		vulkanLogicalDevice();
	}
	~LogicalDevice()
	{
		vkDestroyDevice(vkdevice, nullptr);
	}

	void waitGraphicsQueue()
	{
		vkQueueWaitIdle(graphicsQueue);
	}

	void submitCommandToGraphicsQueue(VkSubmitInfo& submitInfo, VkFence fence = VK_NULL_HANDLE)
	{
		if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, fence) != VK_SUCCESS)
			throw std::runtime_error("Failed to submit draw command buffer");
	}

	void present(VkPresentInfoKHR& presentInfo)
	{
		vkQueuePresentKHR(presentQueue, &presentInfo);
	}
};

struct LowRenderer_Vk::GraphicsPipeline
{
	LowRenderer_Vk* q;

	// TODO : make the pipeline independant in order to make different pipelines
	class IGraphicsPipeline* pipeline = nullptr;
	std::atomic_flag readyToDraw = ATOMIC_FLAG_INIT;
	std::shared_ptr<class Shader> shader = nullptr;

	// swapchain
	VkSwapchainKHR swapchain;
	std::vector<VkImage> swapchainImages;
	VkFormat swapchainImageFormat;
	VkExtent2D swapchainExtent;
	std::vector<VkImageView> swapchainImageViews;

	// pipeline
	VkRenderPass renderPass;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;

	// framebuffer
	// TODO : create framebuffer interface FrameBuffer_Vk : public IFrameBuffer
	std::vector<VkFramebuffer> swapchainFramebuffers;

	// multithreading
	VkSemaphore renderReadySemaphore;
	VkSemaphore renderDoneSemaphore;
	VkFence renderOnceFence;

	GraphicsPipeline(LowRenderer_Vk* q) : q(q)
	{
		shader = ResourcesManager::load<Shader>("triangle_shader", "shaders/triangle", *q);

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
	~GraphicsPipeline()
	{
		vkDeviceWaitIdle(q->devicePImpl->vkdevice);

		vkDestroySemaphore(q->devicePImpl->vkdevice, renderReadySemaphore, nullptr);
		vkDestroySemaphore(q->devicePImpl->vkdevice, renderDoneSemaphore, nullptr);
		vkDestroyFence(q->devicePImpl->vkdevice, renderOnceFence, nullptr);
		for (VkFramebuffer& framebuffer : swapchainFramebuffers)
		{
			vkDestroyFramebuffer(q->devicePImpl->vkdevice, framebuffer, nullptr);
		}
		shader.reset();
		vkDestroyPipeline(q->devicePImpl->vkdevice, graphicsPipeline, nullptr);
		vkDestroyPipelineLayout(q->devicePImpl->vkdevice, pipelineLayout, nullptr);
		vkDestroyRenderPass(q->devicePImpl->vkdevice, renderPass, nullptr);
		for (VkImageView& imageView : swapchainImageViews)
		{
			vkDestroyImageView(q->devicePImpl->vkdevice, imageView, nullptr);
		}
		vkDestroySwapchainKHR(q->devicePImpl->vkdevice, swapchain, nullptr);
	}

	void vulkanSwapchain()
	{
		VkSwapchainSupportDetails support = q->devicePImpl->pdevice.querySwapchainSupport(q->surface);

		VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(support.formats);
		VkPresentModeKHR presentMode = chooseSwapPresentMode(support.presentModes);
		VkExtent2D extent = chooseSwapExtent(support.capabilities);

		uint32_t imageCount = support.capabilities.minImageCount + 1;
		if (support.capabilities.maxImageCount > 0 && support.capabilities.maxImageCount < imageCount)
			imageCount = support.capabilities.maxImageCount;

		VkSwapchainCreateInfoKHR createInfo = {
			.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			.surface = q->surface,
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

		VkQueueFamilyIndices indices = q->devicePImpl->pdevice.findQueueFamilies(q->surface);
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

		if (vkCreateSwapchainKHR(q->devicePImpl->vkdevice, &createInfo, nullptr, &swapchain) != VK_SUCCESS)
			throw std::runtime_error("Failed to create swapchain");

		vkGetSwapchainImagesKHR(q->devicePImpl->vkdevice, swapchain, &imageCount, nullptr);
		swapchainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(q->devicePImpl->vkdevice, swapchain, &imageCount, swapchainImages.data());

		swapchainImageFormat = surfaceFormat.format;
		swapchainExtent = extent;
	}
	void vulkanImageViews()
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

			if (vkCreateImageView(q->devicePImpl->vkdevice, &createInfo, nullptr, &swapchainImageViews[i]) != VK_SUCCESS)
				throw std::runtime_error("Failed to create an image view");
		}
	}

	void vulkanGraphicsPipeline()
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

			if (vkCreatePipelineLayout(q->devicePImpl->vkdevice, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
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

			if (vkCreateGraphicsPipelines(q->devicePImpl->vkdevice, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &graphicsPipeline) != VK_SUCCESS)
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
	void vulkanRenderPass()
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

		if (vkCreateRenderPass(q->devicePImpl->vkdevice, &createInfo, nullptr, &renderPass) != VK_SUCCESS)
			throw std::runtime_error("Failed to create render pass");
	}

	void vulkanFramebuffers()
	{
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

			if (vkCreateFramebuffer(q->devicePImpl->vkdevice, &createInfo, nullptr, &swapchainFramebuffers[i]) != VK_SUCCESS)
				throw std::runtime_error("Failed to create framebuffer");
		}
	}

	void vulkanMultithreadObjects()
	{
		VkSemaphoreCreateInfo semaphoreCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
		};

		VkFenceCreateInfo fenceCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			.flags = VK_FENCE_CREATE_SIGNALED_BIT
		};

		if (vkCreateSemaphore(q->devicePImpl->vkdevice, &semaphoreCreateInfo, nullptr, &renderReadySemaphore) != VK_SUCCESS)
			throw std::runtime_error("Failed to create semaphore");
		if (vkCreateSemaphore(q->devicePImpl->vkdevice, &semaphoreCreateInfo, nullptr, &renderDoneSemaphore) != VK_SUCCESS)
			throw std::runtime_error("Failed to create semaphore");
		if (vkCreateFence(q->devicePImpl->vkdevice, &fenceCreateInfo, nullptr, &renderOnceFence) != VK_SUCCESS)
			throw std::runtime_error("Failed to create fence");
	}

	void recordImageCommandBuffer(CommandBuffer_Vk& cb,
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

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
	{
		for (const VkSurfaceFormatKHR& availableFormat : availableFormats)
		{
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				return availableFormat;
		}

		return availableFormats[0];
	}
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availableModes)
	{
		for (const VkPresentModeKHR& availableMode : availableModes)
		{
			if (availableMode == VK_PRESENT_MODE_MAILBOX_KHR)
				return availableMode;
		}

		return VK_PRESENT_MODE_FIFO_KHR;
	}
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
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

	void draw(CommandBuffer_Vk& cb, const std::deque<std::shared_ptr<IVertexBuffer>>& vbos)
	{
		if (!readyToDraw.test())
			return;

		vkWaitForFences(q->devicePImpl->vkdevice, 1, &renderOnceFence, VK_TRUE, UINT64_MAX);
		vkResetFences(q->devicePImpl->vkdevice, 1, &renderOnceFence);

		uint32_t imageIndex;
		vkAcquireNextImageKHR(q->devicePImpl->vkdevice, swapchain, UINT64_MAX, renderReadySemaphore, VK_NULL_HANDLE, &imageIndex);

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

		q->devicePImpl->submitCommandToGraphicsQueue(submitInfo, renderOnceFence);

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

		q->devicePImpl->present(presentInfo);
	}
};

struct LowRenderer_Vk::CommandPool
{
	LowRenderer_Vk* q;

	VkCommandPool commandPool;
	std::deque<CommandBuffer_Vk> cbos;

	CommandPool(LowRenderer_Vk* q) : q(q)
	{
		vulkanCommandPool();
	}
	~CommandPool()
	{
		for (int i = 0; i < cbos.size(); ++i)
		{
			destroyFloatingCommandBuffer(cbos[i]);
		}
		cbos.clear();

		vkDestroyCommandPool(q->devicePImpl->vkdevice, commandPool, nullptr);
	}

	void vulkanCommandPool()
	{
		VkQueueFamilyIndices indices = q->devicePImpl->pdevice.findQueueFamilies(q->surface);

		VkCommandPoolCreateInfo createInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
			.queueFamilyIndex = indices.graphicsFamily.value()
		};

		if (vkCreateCommandPool(q->devicePImpl->vkdevice, &createInfo, nullptr, &commandPool) != VK_SUCCESS)
			throw std::runtime_error("Failed to create command pool");
	}

	CommandBuffer_Vk createFloatingCommandBuffer()
	{
		VkCommandBufferAllocateInfo allocInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.commandPool = commandPool,
			.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			.commandBufferCount = 1
		};

		CommandBuffer_Vk outCbo;
		if (vkAllocateCommandBuffers(q->devicePImpl->vkdevice, &allocInfo, &outCbo.get()) != VK_SUCCESS)
			throw std::runtime_error("Failed to allocate command buffers");

		return outCbo;
	}
	CommandBuffer_Vk& createCommandBuffer()
	{
		return cbos.emplace_back(createFloatingCommandBuffer());
	}

	void destroyFloatingCommandBuffer(CommandBuffer_Vk& cbo)
	{
		vkFreeCommandBuffers(q->devicePImpl->vkdevice, commandPool, 1, &cbo.get());
	}
	void destroyCommandBuffer(const int index)
	{
		destroyFloatingCommandBuffer(cbos[index]);
		cbos.erase(cbos.begin() + index);
	}
	CommandBuffer_Vk& getCmdBufferByIndex(const int index)
	{
		return cbos[index];
	}
};

struct LowRenderer_Vk::MemoryAllocator : public IMemoryAllocator
{
	LowRenderer_Vk* q;

	MemoryAllocator(LowRenderer_Vk* q) : q(q) { createAllocatorInstance(); }

#ifdef USE_VMA
	VmaAllocator allocator;

	void createAllocatorInstance() override
	{
		VMAHelper::createAllocator(q->instance, q->devicePImpl->pdevice.vkpdevice, q->devicePImpl->vkdevice, allocator);
	}
	void destroyAllocatorInstance() override
	{
		VMAHelper::destroyAllocator(allocator);
	}

	void allocateBufferObjectMemory(VkBufferCreateInfo& createInfo,
		class IVertexBuffer* vbo,
		uint32_t memoryFlags = 0,
		bool mappable = false) override
	{
		VMAHelper::allocateBufferObjectMemory(allocator, createInfo, vbo, mappable);
	}
	void destroyBufferObjectMemory(class IVertexBuffer* vbo) override
	{
		VMAHelper::destroyBufferObjectMemory(allocator, vbo);
	}

	void mapMemory(IAllocation* allocation, void** ppData) override
	{
		VMAHelper::mapMemory(allocator, ((Alloc_VMA*)allocation)->allocation, ppData);
	}
	void unmapMemory(IAllocation* allocation) override
	{
		VMAHelper::unmapMemory(allocator, ((Alloc_VMA*)allocation)->allocation);
	}
#else
	struct MemoryBlock
	{
		// memory allocated on the GPU heap
		VkDeviceMemory memory;

		size_t usedSpace = 0;

		bool operator==(MemoryBlock& other) const
		{
			return memory == other.memory && usedSpace == other.usedSpace;
		}
	};

	// default block size
	size_t blockSize = 1024;
	std::deque<MemoryBlock> memBlocks;

	MemoryBlock& findFirstAvailableBlock(size_t querySize, VkBuffer& buffer, VkMemoryPropertyFlags memProperties)
	{
		// using an existing memory block if possible

		for (int i = 0; i < memBlocks.size(); ++i)
		{
			size_t freeSpace = blockSize - memBlocks[i].usedSpace;
			if (freeSpace >= querySize)
				return memBlocks[i];
		}

		VkMemoryRequirements memReq;
		vkGetBufferMemoryRequirements(q->devicePImpl->vkdevice, buffer, &memReq);

		// VRAM heap
		VkMemoryAllocateInfo allocInfo = {
			.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
			.allocationSize = blockSize,
			.memoryTypeIndex = q->devicePImpl->pdevice.findMemoryType(memReq.memoryTypeBits, memProperties)
		};

		// adding a new memory block

		MemoryBlock newBlock;
		if (vkAllocateMemory(q->devicePImpl->vkdevice, &allocInfo, nullptr, &newBlock.memory) != VK_SUCCESS)
			throw std::runtime_error("Failed to allocate vertex buffer memory");

		return memBlocks.emplace_back(newBlock);
	}

	void createAllocatorInstance() override {}
	void destroyAllocatorInstance() override
	{
		for (int i = 0; i < memBlocks.size(); ++i)
		{
			vkFreeMemory(q->devicePImpl->vkdevice, memBlocks[i].memory, nullptr);
		}
		memBlocks.clear();
	}

	void allocateBufferObjectMemory(VkBufferCreateInfo& createInfo,
		class IVertexBuffer* vbo,
		uint32_t memoryFlags = 0,
		bool mappable = false) override
	{
		VertexBuffer_Vk* vk = (VertexBuffer_Vk*)vbo;

		if (vkCreateBuffer(q->devicePImpl->vkdevice, &createInfo, nullptr, &vk->buffer) != VK_SUCCESS)
			throw std::runtime_error("Failed to create vertex buffer");

		Alloc* alloc = (Alloc*)vk->alloc;

		// binding memory block
		MemoryBlock& block = findFirstAvailableBlock(vk->bufferSize, vk->buffer, memoryFlags);
		alloc->memoryOffset = block.usedSpace;
		vkBindBufferMemory(q->devicePImpl->vkdevice, vk->buffer, block.memory, block.usedSpace);

		// marking space as taken
		block.usedSpace += vk->bufferSize;
		alloc->memoryBlock = &block;
	}
	void destroyBufferObjectMemory(class IVertexBuffer* vbo) override
	{
		VertexBuffer_Vk* vk = (VertexBuffer_Vk*)vbo;

		MemoryBlock* currentBlock = ((Alloc*)vk->alloc)->memoryBlock;
		currentBlock->usedSpace -= vk->bufferSize;
		if (currentBlock->usedSpace <= 0)
		{
			vkFreeMemory(q->devicePImpl->vkdevice, currentBlock->memory, nullptr);
			memBlocks.erase(std::remove(memBlocks.begin(), memBlocks.end(), *currentBlock));
		}
	}

	void mapMemory(IAllocation* allocation, void** ppData) override
	{
		Alloc* alloc = (Alloc*)allocation;
		// mapping the whole block
		vkMapMemory(q->devicePImpl->vkdevice, alloc->memoryBlock->memory, 0, blockSize, 0, ppData);

		// mapping vertex buffer's offset and size
		//vkMapMemory(device->vkdevice, alloc->memoryBlock->memory, vbo.memoryOffset, vbo.bufferSize, 0, ppData);
	}
	void unmapMemory(IAllocation* allocation) override
	{
		Alloc* alloc = (Alloc*)allocation;
		vkUnmapMemory(q->devicePImpl->vkdevice, alloc->memoryBlock->memory);
	}
#endif
};


// args[0] should be additionalExtensions
void LowRenderer_Vk::initGraphicsAPI_Impl(std::span<void*> args)
{
	this->additionalExtensions = *(std::vector<const char*>*)args[0];

	if (!gladLoaderLoadVulkan(nullptr, nullptr, nullptr))
		throw std::runtime_error("Unable to load Vulkan symbols");

	// check extensions and layers
	vulkanExtensions();
	vulkanLayers();

	// create the Vulkan instance
	vulkanCreate();
#ifndef NDEBUG
	vulkanDebugMessenger();
#endif
}

void LowRenderer_Vk::initRendererModules()
{
	devicePImpl = std::make_unique<LogicalDevice>(this);
	pipelinePImpl = std::make_unique<GraphicsPipeline>(this);
	commandPoolPImpl = std::make_unique<CommandPool>(this);
	allocatorPImpl = std::make_unique<MemoryAllocator>(this);

	// default command buffer
	commandPoolPImpl->createCommandBuffer();
}
void LowRenderer_Vk::terminateRendererModules()
{
	pipelinePImpl.reset();
	commandPoolPImpl.reset();

	for (int i = 0; i < vbos.size(); ++i)
	{
		destroyBufferObject(*vbos[i]);
	}
	vbos.clear();
	ResourcesManager::clearAllResources();

	allocatorPImpl.reset();
	devicePImpl.reset();
}

void LowRenderer_Vk::terminateGraphicsAPI()
{
	terminateRendererModules();
	vulkanDestroy();
}

VKAPI_ATTR VkBool32 VKAPI_CALL LowRenderer_Vk::debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
	void* userData)
{
	std::cerr << "validation layer: " << callbackData->pMessage << std::endl;
	return VK_FALSE;
}

void LowRenderer_Vk::vulkanDestroy()
{
	vkDestroySurfaceKHR(instance, surface, nullptr);
	vkDestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
	vkDestroyInstance(instance, nullptr);
}

void LowRenderer_Vk::vulkanCreate()
{
	VkApplicationInfo appInfo = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = "renderer",
		.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
		.engineVersion = VK_MAKE_VERSION(1, 0, 0),
		.apiVersion = VK_API_VERSION_1_3
	};

#ifndef NDEBUG
	additionalExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

	VkInstanceCreateInfo createInfo = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo = &appInfo,
#ifdef NDEBUG
		.enabledLayerCount = 0,
#else
		.enabledLayerCount = static_cast<uint32_t>(validationLayers.size()),
		.ppEnabledLayerNames = validationLayers.data(),
#endif
		.enabledExtensionCount = static_cast<uint32_t>(additionalExtensions.size()),
		.ppEnabledExtensionNames = additionalExtensions.data()
	};

	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
		throw std::runtime_error("Failed to create Vulkan instance");

	if (!gladLoaderLoadVulkan(instance, nullptr, nullptr))
		throw std::runtime_error("Unable to reload Vulkan symbols with Vulkan instance");
}

void LowRenderer_Vk::vulkanDebugMessenger()
{
	VkDebugUtilsMessengerCreateInfoEXT createInfo = {
		.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
		.messageSeverity =
		//VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT
		,
	.messageType =
		VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT
		,
	.pfnUserCallback = debugCallback,
	.pUserData = nullptr
	};

	if (vkCreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS)
		throw std::runtime_error("Failed to set up debug messenger");
}

void LowRenderer_Vk::vulkanExtensions()
{
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
	std::cout << "available instance extensions : " << extensionCount << '\n';
	for (const auto& extension : extensions)
		std::cout << '\t' << extension.extensionName << '\n';
}

void LowRenderer_Vk::vulkanLayers()
{
	uint32_t layerCount = 0;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	std::vector<VkLayerProperties> layers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, layers.data());
	std::cout << "available layers : " << layerCount << '\n';
	for (const auto& layer : layers)
		std::cout << '\t' << layer.layerName << '\n';
}


// additionalArgs[0] must be "VkBufferUsageFlags usage"
// additionalArgs[1] must be "VkMemoryPropertyFlags memProperties"
[[nodiscard]] std::shared_ptr<IVertexBuffer> LowRenderer_Vk::createBufferObject_Impl(uint32_t vertexNum,
	bool mappable,
	std::span<uint32_t> additionalArgs)
{
	VkBufferUsageFlags usage = additionalArgs[0];
	VkMemoryPropertyFlags memProperties = additionalArgs[1];

	// out buffer object
	std::shared_ptr<IVertexBuffer> outVbo = IVertexBuffer::instantiate(vertexNum, EGraphicsAPI::VULKAN);

	VkBufferCreateInfo createInfo = {
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.flags = 0,
		.size = (VkDeviceSize)outVbo->bufferSize,
		.usage = usage,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE
	};

	allocatorPImpl->allocateBufferObjectMemory(createInfo, outVbo.get(), memProperties, mappable);

	return outVbo;
}

void LowRenderer_Vk::populateBufferObject(IVertexBuffer& vbo, const Vertex* vertices)
{
	VertexBuffer_Vk& vk = (VertexBuffer_Vk&)vbo;

	// populating the VBO (using a CPU accessible memory)
	allocatorPImpl->mapMemory(vk.alloc, &vk.vertices);

	// TODO : flush memory
	memcpy(vk.vertices, vertices, vk.bufferSize);
	// TODO : invalidate memory before reading in the pipeline

	allocatorPImpl->unmapMemory(vk.alloc);
}

void LowRenderer_Vk::destroyBufferObject(IVertexBuffer& vbo)
{
	VertexBuffer_Vk& vk = (VertexBuffer_Vk&)vbo;

	vkDestroyBuffer(devicePImpl->vkdevice, vk.buffer, nullptr);
	allocatorPImpl->destroyBufferObjectMemory(&vbo);
}


std::shared_ptr<IVertexBuffer> LowRenderer_Vk::createVertexBuffer_Impl(uint32_t vertexNum, const Vertex* vertices)
{
	// this buffer is a CPU accessible buffer (temporary buffer to later load the data to the GPU)
	std::shared_ptr<VertexBuffer_Vk> stagingVBO =
		std::dynamic_pointer_cast<VertexBuffer_Vk>(createBufferObject(vertexNum,
			true,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,	// used for memory transfer operation
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));

	// populating the CPU accessible buffer
	populateBufferObject(*stagingVBO, vertices);

	// creating a device (GPU) local buffer (on the specific device of the renderer)
	// store this vertex buffer to keep a reference
	std::shared_ptr<VertexBuffer_Vk> vbo =
		std::dynamic_pointer_cast<VertexBuffer_Vk>(createBufferObject(vertexNum,
			false,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,	// memory transfer operation
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));

	// copying the staging buffer data into the device local buffer

	// using a command buffer to transfer the data
	CommandBuffer_Vk cbo = commandPoolPImpl->createFloatingCommandBuffer();

	// copy the staging buffer (CPU accessible) into the GPU buffer (GPU memory)
	cbo.beginRecord(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	VkBufferCopy copyRegion = {
		.srcOffset = 0,
		.dstOffset = 0,
		.size = stagingVBO->bufferSize
	};
	vkCmdCopyBuffer(cbo.get(), stagingVBO->buffer, vbo->buffer, 1, &copyRegion);

	cbo.endRecord();

	VkSubmitInfo submitInfo = {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.commandBufferCount = 1,
		.pCommandBuffers = &cbo.get()
	};
	devicePImpl->submitCommandToGraphicsQueue(submitInfo);
	devicePImpl->waitGraphicsQueue();

	commandPoolPImpl->destroyFloatingCommandBuffer(cbo);
	destroyBufferObject(*stagingVBO);

	return vbo;
}

std::shared_ptr<IShaderModule> LowRenderer_Vk::createShaderModule_Impl(size_t vsSize,
	size_t fsSize,
	char* vs,
	char* fs)
{
	std::shared_ptr<ShaderModule_Vk> sh =
		std::dynamic_pointer_cast<ShaderModule_Vk>(IShaderModule::instantiate(EGraphicsAPI::VULKAN));

	auto createShaderModule = [&](char* code, size_t codeSize) {
		VkShaderModuleCreateInfo createInfo = {
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.codeSize = codeSize,
		.pCode = reinterpret_cast<const uint32_t*>(code)
		};

		VkShaderModule shaderModule;
		if (vkCreateShaderModule(devicePImpl->vkdevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
			throw std::runtime_error("Failed to create shader module");
		return shaderModule;
	};

	sh->vsModule = createShaderModule(vs, vsSize);
	sh->fsModule = createShaderModule(fs, fsSize);

	return sh;
}

void LowRenderer_Vk::destroyShaderModule_Impl(std::shared_ptr<class IShaderModule> ptr)
{
	std::shared_ptr<ShaderModule_Vk> sh = std::dynamic_pointer_cast<ShaderModule_Vk>(ptr);
	vkDestroyShaderModule(devicePImpl->vkdevice, sh->vsModule, nullptr);
	vkDestroyShaderModule(devicePImpl->vkdevice, sh->fsModule, nullptr);
}

IVertexBuffer& LowRenderer_Vk::addVBO(std::shared_ptr<IVertexBuffer> vbo)
{
	// TODO : add mutex for thread safety
	return *vbos.emplace_back(vbo);
}

void LowRenderer_Vk::renderFrame()
{
	pipelinePImpl->draw(commandPoolPImpl->getCmdBufferByIndex(0), vbos);
}

#ifdef USE_VMA
#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>
#endif