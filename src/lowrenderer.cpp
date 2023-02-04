#include <iostream>

#include "lowrenderer.hpp"

void LowRenderer::create()
{
	// check extensions and layers
	vulkanExtensions();
	vulkanLayers();

	// create the Vulkan instance
	vulkanCreate();
#ifndef NDEBUG
	vulkanDebugMessenger();
#endif
}

void LowRenderer::destroy()
{
	vulkanDestroy();
}

VKAPI_ATTR VkBool32 VKAPI_CALL LowRenderer::debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
	void* userData)
{
	std::cerr << "validation layer: " << callbackData->pMessage << std::endl;
	return VK_FALSE;
}

void LowRenderer::vulkanDestroy()
{
	vkDestroySurfaceKHR(instance, surface, nullptr);
	vkDestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
	vkDestroyInstance(instance, nullptr);
}

void LowRenderer::vulkanCreate()
{
	VkApplicationInfo appInfo = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = "renderer",
		.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
		.engineVersion = VK_MAKE_VERSION(1, 0, 0),
		.apiVersion = VK_API_VERSION_1_0
	};

	// TODO : remove reference to GLFW
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;

	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> enabledExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
#ifndef NDEBUG
	enabledExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
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
		.enabledExtensionCount = static_cast<uint32_t>(enabledExtensions.size()),
		.ppEnabledExtensionNames = enabledExtensions.data()
	};

	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
		throw std::exception("Failed to create Vulkan instance");

	if (!gladLoaderLoadVulkan(instance, nullptr, nullptr))
		throw std::exception("Unable to reload Vulkan symbols with Vulkan instance");
}

void LowRenderer::vulkanDebugMessenger()
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
		throw std::exception("Failed to set up debug messenger");
}

void LowRenderer::vulkanExtensions()
{
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
	std::cout << "available instance extensions : " << extensionCount << '\n';
	for (const auto& extension : extensions)
		std::cout << '\t' << extension.extensionName << '\n';
}

void LowRenderer::vulkanLayers()
{
	uint32_t layerCount = 0;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	std::vector<VkLayerProperties> layers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, layers.data());
	std::cout << "available layers : " << layerCount << '\n';
	for (const auto& layer : layers)
		std::cout << '\t' << layer.layerName << '\n';
}