#pragma once

#include <iostream>
#include <vector>

#include <glad/vulkan.h>

class Context
{
public:
#ifndef NDEBUG
	static constexpr const uint32_t validationLayerCount = 1;
	static constexpr const char* validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};
#endif

private:
	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
		void* userData)
	{
		std::cerr << "[Validation Layer] : " << callbackData->pMessage << std::endl;
		return VK_FALSE;
	}


public:
	Context() = delete;
	Context(const char* applicationName,
		const uint32_t applicationVersion,
		const uint32_t engineVersion,
		std::vector<const char*> additionalExtensions)
	{
		VkApplicationInfo appInfo = {
			.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.pApplicationName = applicationName,
			.applicationVersion = applicationVersion,
			.engineVersion = engineVersion,
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
			.enabledLayerCount = validationLayerCount,
			.ppEnabledLayerNames = &validationLayers,
#endif
			.enabledExtensionCount = static_cast<uint32_t>(additionalExtensions.size()),
			.ppEnabledExtensionNames = additionalExtensions.data()
		};

		if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
			throw std::runtime_error("Failed to create Vulkan instance");
	}
	~Context()
	{
		vkDestroyInstance(instance, nullptr);
	}


	void createDebugMessenger()
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
	void destroyDebugMessenger()
	{
		vkDestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
	}
};