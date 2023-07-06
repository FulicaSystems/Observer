#include <iostream>
#include <stdexcept>

#include "memorymanager.hpp"

#include "renderer.hpp"

#include "commandpool_vk.hpp"
#include "commandbuffer_vk.hpp"

#include "lowrenderer_vk.hpp"
#include "graphicsdevice_vk.hpp"

#include "vertex.hpp"
#include "vertexbuffer_vk.hpp"
#include "shadermodule_vk.hpp"

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

void LowRenderer_Vk::terminateGraphicsAPI()
{
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

	highRenderer->allocator->allocateBufferObjectMemory(createInfo, outVbo.get(), memProperties, mappable);

	return outVbo;
}

void LowRenderer_Vk::populateBufferObject(IVertexBuffer& vbo, const Vertex* vertices)
{
	VertexBuffer_Vk& vk = (VertexBuffer_Vk&)vbo;

	// populating the VBO (using a CPU accessible memory)
	highRenderer->allocator->mapMemory(vk.alloc, &vk.vertices);

	// TODO : flush memory
	memcpy(vk.vertices, vertices, vk.bufferSize);
	// TODO : invalidate memory before reading in the pipeline

	highRenderer->allocator->unmapMemory(vk.alloc);
}

void LowRenderer_Vk::destroyBufferObject(IVertexBuffer& vbo)
{
	VertexBuffer_Vk& vk = (VertexBuffer_Vk&)vbo;

	vkDestroyBuffer(((LogicalDevice_Vk*)highRenderer->device)->vkdevice, vk.buffer, nullptr);
	highRenderer->allocator->destroyBufferObjectMemory(&vbo);
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
	CommandBuffer_Vk cbo = ((CommandPool_Vk*)highRenderer->commandPool)->createFloatingCommandBuffer();

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
	((LogicalDevice_Vk*)highRenderer->device)->submitCommandToGraphicsQueue(submitInfo);
	((LogicalDevice_Vk*)highRenderer->device)->waitGraphicsQueue();

	((CommandPool_Vk*)highRenderer->commandPool)->destroyFloatingCommandBuffer(cbo);
	destroyBufferObject(*stagingVBO);

	return vbo;
}

std::shared_ptr<IShaderModule> LowRenderer_Vk::createShaderModule_Impl(ILogicalDevice* device,
	size_t vsSize,
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

		VkDevice vkdevice = ((LogicalDevice_Vk*)highRenderer->device)->vkdevice;
		VkShaderModule shaderModule;
		if (vkCreateShaderModule(vkdevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
			throw std::runtime_error("Failed to create shader module");
		return shaderModule;
	};

	sh->vsModule = createShaderModule(vs, vsSize);
	sh->fsModule = createShaderModule(fs, fsSize);

	return sh;
}

void LowRenderer_Vk::destroyShaderModule_Impl(std::shared_ptr<class IShaderModule> ptr)
{
	LogicalDevice_Vk& device = (LogicalDevice_Vk&)*highRenderer->device;
	std::shared_ptr<ShaderModule_Vk> sh = std::dynamic_pointer_cast<ShaderModule_Vk>(ptr);
	vkDestroyShaderModule(device.vkdevice, sh->vsModule, nullptr);
	vkDestroyShaderModule(device.vkdevice, sh->fsModule, nullptr);
}

std::shared_ptr<class ILogicalDevice> LowRenderer_Vk::createLogicalDevice_Impl()
{
	return std::shared_ptr<class ILogicalDevice>();
}

void LowRenderer_Vk::destroyLogicalDevice_Impl(std::shared_ptr<class ILogicalDevice> ptr)
{
	vkDestroyDevice(std::dynamic_pointer_cast<LogicalDevice_Vk>(ptr)->vkdevice, nullptr);
}
