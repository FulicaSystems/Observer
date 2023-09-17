#include <iostream>
#include <stdexcept>
#include <set>

#include <glad/vulkan.h>


#include "format.hpp"
#include "utils/multithread/globalthreadpool.hpp"

#include "resourcesmanager.hpp"
#include "memoryallocator.hpp"


#include "vertex.hpp"
#include "vertexbuffer_vk.hpp"
#include "shader.hpp"
#include "shadermodule_vk.hpp"


#include "lowrenderer_vk.hpp"


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


void LogicalDevice::waitGraphicsQueue()
{
	vkQueueWaitIdle(graphicsQueue);
}
void LogicalDevice::submitCommandToGraphicsQueue(VkSubmitInfo& submitInfo, VkFence fence)
{
	if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, fence) != VK_SUCCESS)
		throw std::runtime_error("Failed to submit draw command buffer");
}
void LogicalDevice::present(VkPresentInfoKHR& presentInfo)
{
	vkQueuePresentKHR(presentQueue, &presentInfo);
}


void GraphicsPipeline::recordImageCommandBuffer(CommandBuffer& cb,
	uint32_t imageIndex,
	const std::deque<std::shared_ptr<class IVertexBuffer>>& vbos)
{
	cb.reset();
	cb.beginRecord();

	VkCommandBuffer& cbo = cb.commandBuffer;

	VkClearValue clearColor = {
		.color = { 0.2f, 0.2f, 0.2f, 1.f }
	};

	VkRenderPassBeginInfo renderPassBeginInfo = {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.renderPass = renderPass,
		.framebuffer = framebuffers.swapchainFramebuffers[imageIndex],
		.renderArea = {
			.offset = { 0, 0 },
			.extent = swapchain.swapchainExtent
			},
		.clearValueCount = 1,
		.pClearValues = &clearColor
	};

	vkCmdBeginRenderPass(cbo, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline(cbo, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

	VkViewport viewport = {
		.x = 0.f,
		.y = 0.f,
		.width = static_cast<float>(swapchain.swapchainExtent.width),
		.height = static_cast<float>(swapchain.swapchainExtent.height),
		.minDepth = 0.f,
		.maxDepth = 1.f
	};

	vkCmdSetViewport(cbo, 0, 1, &viewport);

	VkRect2D scissor = {
		.offset = { 0, 0 },
		.extent = swapchain.swapchainExtent
	};

	vkCmdSetScissor(cbo, 0, 1, &scissor);

	// bind VBOs
	// TODO : loop command buffer usage instead of looping vbos as a single command
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


void CommandBuffer::reset()
{
	vkResetCommandBuffer(commandBuffer, 0);
}
void CommandBuffer::beginRecord(VkCommandBufferUsageFlags flags)
{
	VkCommandBufferBeginInfo commandBufferBeginInfo = {
	.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
	.flags = flags,
	.pInheritanceInfo = nullptr
	};

	if (vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo) != VK_SUCCESS)
		throw std::runtime_error("Failed to begin recording command buffer");
}
void CommandBuffer::endRecord()
{
	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		throw std::runtime_error("Failed to record command buffer");
}

CommandBuffer& CommandPool::getCmdBufferByIndex(const int index)
{
	return cbos[index];
}


void LowRenderer_Vk::createSwapchainImageViews(LogicalDevice logicalDevice, Swapchain& swapchain)
{
	swapchain.swapchainImageViews.resize(swapchain.swapchainImages.size());

	for (size_t i = 0; i < swapchain.swapchainImages.size(); ++i)
	{
		VkImageViewCreateInfo createInfo = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.image = swapchain.swapchainImages[i],
			.viewType = VK_IMAGE_VIEW_TYPE_2D,
			.format = swapchain.swapchainImageFormat,
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

		if (vkCreateImageView(logicalDevice.vkdevice, &createInfo, nullptr, &swapchain.swapchainImageViews[i]) != VK_SUCCESS)
			throw std::runtime_error("Failed to create an image view");
	}
}


void LowRenderer_Vk::createPipelineMultithreadObjects(LogicalDevice logicalDevice, GraphicsPipeline& pipeline)
{
	VkSemaphoreCreateInfo semaphoreCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
	};

	VkFenceCreateInfo fenceCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		.flags = VK_FENCE_CREATE_SIGNALED_BIT
	};

	if (vkCreateSemaphore(logicalDevice.vkdevice, &semaphoreCreateInfo, nullptr, &pipeline.renderReadySemaphore) != VK_SUCCESS)
		throw std::runtime_error("Failed to create semaphore");
	if (vkCreateSemaphore(logicalDevice.vkdevice, &semaphoreCreateInfo, nullptr, &pipeline.renderDoneSemaphore) != VK_SUCCESS)
		throw std::runtime_error("Failed to create semaphore");
	if (vkCreateFence(logicalDevice.vkdevice, &fenceCreateInfo, nullptr, &pipeline.renderOnceFence) != VK_SUCCESS)
		throw std::runtime_error("Failed to create fence");
}


CommandPool LowRenderer_Vk::createCommandPool(LogicalDevice logicalDevice)
{
	VkQueueFamilyIndices indices = logicalDevice.pdevice.findQueueFamilies(surface);

	VkCommandPoolCreateInfo createInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		.queueFamilyIndex = indices.graphicsFamily.value()
	};

	CommandPool commandPool;
	if (vkCreateCommandPool(logicalDevice.vkdevice, &createInfo, nullptr, &commandPool.commandPool) != VK_SUCCESS)
		throw std::runtime_error("Failed to create command pool");

	return commandPool;
}
void LowRenderer_Vk::destroyCommandPool(LogicalDevice logicalDevice, CommandPool& commandPool)
{
	for (int i = 0; i < commandPool.cbos.size(); ++i)
	{
		destroyFloatingCommandBuffer(commandPool.cbos[i]);
	}
	commandPool.cbos.clear();

	vkDestroyCommandPool(logicalDevice.vkdevice, commandPool.commandPool, nullptr);
}


CommandBuffer LowRenderer_Vk::createFloatingCommandBuffer()
{
	VkCommandBufferAllocateInfo allocInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.commandPool = commandPool.commandPool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = 1
	};

	CommandBuffer outCbo;
	if (vkAllocateCommandBuffers(logicalDevice.vkdevice, &allocInfo, &outCbo.commandBuffer) != VK_SUCCESS)
		throw std::runtime_error("Failed to allocate command buffers");

	return outCbo;
}
void LowRenderer_Vk::destroyFloatingCommandBuffer(CommandBuffer& cbo)
{
	vkFreeCommandBuffers(logicalDevice.vkdevice, commandPool.commandPool, 1, &cbo.commandBuffer);
}
CommandBuffer& LowRenderer_Vk::createCommandBuffer()
{
	return commandPool.cbos.emplace_back(createFloatingCommandBuffer());
}
void LowRenderer_Vk::destroyCommandBuffer(const int index)
{
	destroyFloatingCommandBuffer(commandPool.cbos[index]);
	commandPool.cbos.erase(commandPool.cbos.begin() + index);
}


void LowRenderer_Vk::renderFrame()
{
	if (!pipeline->readyToDraw.test())
		return;

	vkWaitForFences(logicalDevice.vkdevice, 1, &pipeline->renderOnceFence, VK_TRUE, UINT64_MAX);
	vkResetFences(logicalDevice.vkdevice, 1, &pipeline->renderOnceFence);

	uint32_t imageIndex;
	vkAcquireNextImageKHR(logicalDevice.vkdevice, pipeline->swapchain.vkswapchain, UINT64_MAX, pipeline->renderReadySemaphore, VK_NULL_HANDLE, &imageIndex);


	CommandBuffer cb = commandPool.getCmdBufferByIndex(0);
	pipeline->recordImageCommandBuffer(cb, imageIndex, vbos);

	VkSemaphore waitSemaphores[] = { pipeline->renderReadySemaphore };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	VkSemaphore signalSemaphores[] = { pipeline->renderDoneSemaphore };
	VkSubmitInfo submitInfo = {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = waitSemaphores,
		.pWaitDstStageMask = waitStages,
		.commandBufferCount = 1,
		.pCommandBuffers = &cb.commandBuffer,
		.signalSemaphoreCount = 1,
		.pSignalSemaphores = signalSemaphores
	};

	logicalDevice.submitCommandToGraphicsQueue(submitInfo, pipeline->renderOnceFence);

	VkSwapchainKHR swapchains[] = { pipeline->swapchain.vkswapchain };
	VkPresentInfoKHR presentInfo = {
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = signalSemaphores,
		.swapchainCount = 1,
		.pSwapchains = swapchains,
		.pImageIndices = &imageIndex,
		.pResults = nullptr
	};

	logicalDevice.present(presentInfo);
}



// args[0] should be additionalExtensions
void LowRenderer_Vk::initGraphicsAPI_Impl(std::span<void*> args)
{
	this->additionalExtensions = *(std::vector<const char*>*)args[0];

	if (!gladLoaderLoadVulkan(nullptr, nullptr, nullptr))
		throw std::runtime_error("Unable to load Vulkan symbols");

	// check extensions and layers
	printAvailableExtensions();
	printAvailableLayers();

	// create the Vulkan instance
	createAPIInstance("renderer");
#ifndef NDEBUG
	createDebugMessenger();
#endif
}
void LowRenderer_Vk::terminateGraphicsAPI()
{
	terminateRendererModules();
	destroyAPIInstance();
}

void LowRenderer_Vk::initRendererModules()
{
	// device
	logicalDevice = createLogicalDevice(createPhysicalDevice());

	// pipeline
	pipeline = std::make_shared<GraphicsPipeline>();
	// TODO : move shader creation
	pipeline->shaderProgram = ResourcesManager::load<Shader>("triangle_shader", "shaders/triangle", *this);
	pipeline->swapchain = createSwapchain(logicalDevice);
	createSwapchainImageViews(logicalDevice, pipeline->swapchain);
	createPipelineRenderPass(logicalDevice, *pipeline);
	createGraphicsPipelineAsync(logicalDevice, pipeline.get());
	createPipelineFramebuffers(logicalDevice, *pipeline);
	createPipelineMultithreadObjects(logicalDevice, *pipeline);

	// command pool
	commandPool = createCommandPool(logicalDevice);

	// allocator
#ifdef USE_VMA
	memoryAllocator = std::make_shared<VMAMemoryAllocator>();
#else
	memoryAllocator = std::make_shared<MyMemoryAllocator>();
#endif
	memoryAllocator->createAllocatorInstance(instance, logicalDevice.pdevice.vkpdevice, logicalDevice.vkdevice);

	// default command buffer
	createCommandBuffer();
}
void LowRenderer_Vk::terminateRendererModules()
{
	destroyGraphicsPipeline(logicalDevice, *pipeline);
	destroyCommandPool(logicalDevice, commandPool);

	for (int i = 0; i < vbos.size(); ++i)
	{
		destroy<IVertexBuffer>(vbos[i]);
	}
	vbos.clear();
	ResourcesManager::clearAllResources();

	memoryAllocator->destroyAllocatorInstance(logicalDevice.vkdevice);
	memoryAllocator.reset();
	destroyLogicalDevice(logicalDevice);
}

VkSurfaceFormatKHR LowRenderer_Vk::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	for (const VkSurfaceFormatKHR& availableFormat : availableFormats)
	{
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			return availableFormat;
	}

	return availableFormats[0];
}
VkPresentModeKHR LowRenderer_Vk::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availableModes)
{
	for (const VkPresentModeKHR& availableMode : availableModes)
	{
		if (availableMode == VK_PRESENT_MODE_MAILBOX_KHR)
			return availableMode;
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}
VkExtent2D LowRenderer_Vk::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
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

void LowRenderer_Vk::createDebugMessenger()
{
	VkDebugUtilsMessengerCreateInfoEXT createInfo = {
		.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
		.messageSeverity =
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
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
void LowRenderer_Vk::printAvailableExtensions()
{
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
	std::cout << "available instance extensions : " << extensionCount << '\n';
	for (const auto& extension : extensions)
		std::cout << '\t' << extension.extensionName << '\n';
}
void LowRenderer_Vk::printAvailableLayers()
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
std::shared_ptr<IVertexBuffer> LowRenderer_Vk::createVertexBufferObject_Impl(uint32_t vertexNum,
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

	memoryAllocator->allocateBufferObjectMemory(logicalDevice.vkdevice, logicalDevice.pdevice, createInfo, outVbo.get(), memProperties, mappable);

	return outVbo;
}
void LowRenderer_Vk::populateVertexBufferObject(IVertexBuffer& vbo, const Vertex* vertices)
{
	VertexBuffer_Vk& vk = (VertexBuffer_Vk&)vbo;

	// populating the VBO (using a CPU accessible memory)
	memoryAllocator->mapMemory(logicalDevice.vkdevice, vk.alloc, &vk.vertices);

	// TODO : flush memory
	memcpy(vk.vertices, vertices, vk.bufferSize);
	// TODO : invalidate memory before reading in the pipeline

	memoryAllocator->unmapMemory(logicalDevice.vkdevice, vk.alloc);
}
std::shared_ptr<IVertexBuffer> LowRenderer_Vk::createVertexBuffer_Impl(uint32_t vertexNum, const Vertex* vertices)
{
	// this buffer is a CPU accessible buffer (temporary buffer to later load the data to the GPU)
	std::shared_ptr<VertexBuffer_Vk> stagingVBO =
		std::dynamic_pointer_cast<VertexBuffer_Vk>(createVertexBufferObject(vertexNum,
			true,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,	// used for memory transfer operation
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));

	// populating the CPU accessible buffer
	populateVertexBufferObject(*stagingVBO, vertices);

	// creating a device (GPU) local buffer (on the specific device of the renderer)
	// store this vertex buffer to keep a reference
	std::shared_ptr<VertexBuffer_Vk> vbo =
		std::dynamic_pointer_cast<VertexBuffer_Vk>(createVertexBufferObject(vertexNum,
			false,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,	// memory transfer operation
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));

	// copying the staging buffer data into the device local buffer

	// using a command buffer to transfer the data
	CommandBuffer cbo = createFloatingCommandBuffer();

	// copy the staging buffer (CPU accessible) into the GPU buffer (GPU memory)
	cbo.beginRecord(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	VkBufferCopy copyRegion = {
		.srcOffset = 0,
		.dstOffset = 0,
		.size = stagingVBO->bufferSize
	};
	vkCmdCopyBuffer(cbo.commandBuffer, stagingVBO->buffer, vbo->buffer, 1, &copyRegion);

	cbo.endRecord();

	VkSubmitInfo submitInfo = {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.commandBufferCount = 1,
		.pCommandBuffers = &cbo.commandBuffer
	};
	logicalDevice.submitCommandToGraphicsQueue(submitInfo);
	logicalDevice.waitGraphicsQueue();

	destroyFloatingCommandBuffer(cbo);
	destroy<IVertexBuffer>(stagingVBO);

	return vbo;
}
void LowRenderer_Vk::destroyVertexBuffer_Impl(std::shared_ptr<IVertexBuffer> ptr)
{
	std::shared_ptr<VertexBuffer_Vk> vk = std::dynamic_pointer_cast<VertexBuffer_Vk>(ptr);

	vkDestroyBuffer(logicalDevice.vkdevice, vk->buffer, nullptr);
	memoryAllocator->destroyBufferObjectMemory(logicalDevice.vkdevice, ptr.get());
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
		if (vkCreateShaderModule(logicalDevice.vkdevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
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
	vkDestroyShaderModule(logicalDevice.vkdevice, sh->vsModule, nullptr);
	vkDestroyShaderModule(logicalDevice.vkdevice, sh->fsModule, nullptr);
}

IVertexBuffer& LowRenderer_Vk::addVBO(std::shared_ptr<IVertexBuffer> vbo)
{
	// TODO : add mutex for thread safety
	return *vbos.emplace_back(vbo);
}