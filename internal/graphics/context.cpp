#pragma once


#include "context.hpp"


	Context::Context(const char* applicationName,
		const version applicationVersion,
		const version engineVersion,
		std::vector<const char*> additionalExtensions)
	{
            m_loader = std::make_unique<Utils::bin::DynamicLibraryLoader>("vulkan-1");
    vkCreateInstance = (PFN_vkCreateInstance)m_loader->getProcAddr("vkCreateInstance");

    m_instance = std::make_unique<Instance>(*this);
	}
	Context::~Context()
	{
		deviceSelector.reset();
		vkDestroyInstance(instance, nullptr);
	}


	void Context::createDebugMessenger()
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
	void Context::destroyDebugMessenger()
	{
		vkDestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
	}