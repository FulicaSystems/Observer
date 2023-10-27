#pragma once

#include <iostream>
#include <vector>

#include <glad/vulkan.h>


// 32 bits
// 4 bits for major, 12 bits for minor, 16 bits for patch
// MMMM mmmm mmmm mmmm pppp pppp pppp pppp
#define VERSION(major, minor, patch) (uint32_t)(major << 28U | minor << 16U | patch << 0U)
#define MAJOR(version) (uint32_t)((version >> 28U) & 0xfU)
#define MINOR(version) (uint32_t)((version >> 16U) & 0x0fffU)
#define PATCH(version) (uint32_t)((version >> 0U) & 0xfU)
struct version
{
	uint32_t major : 4;
	uint32_t minor : 12;
	uint32_t patch : 16;
};



class Context
{
public:
	static constexpr const uint32_t layerCount = 1;
	static constexpr const char* layers = {
		// TODO : use validation only if !NDEBUG
		"VK_LAYER_KHRONOS_validation" // validation layer
	};

public:
	VkInstance instance;
private:
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
			.applicationVersion = VK_MAKE_API_VERSION(0,
				MAJOR(applicationVersion),
				MINOR(applicationVersion),
				PATCH(applicationVersion)),
			.engineVersion = VK_MAKE_API_VERSION(0,
				MAJOR(engineVersion),
				MINOR(engineVersion),
				PATCH(engineVersion)),
			.apiVersion = VK_API_VERSION_1_3
		};

#ifndef NDEBUG
		additionalExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

		VkInstanceCreateInfo createInfo = {
			.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			.pApplicationInfo = &appInfo,
			.enabledLayerCount = layerCount,
			.ppEnabledLayerNames = &layers,
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