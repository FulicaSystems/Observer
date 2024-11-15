#pragma once

#include "context.hpp"

Context::Context(const char* applicationName,
	const version applicationVersion,
	const version engineVersion,
	std::vector<const char*> additionalExtensions)
	: m_applicationName(applicationName), m_applicationVersion(applicationVersion), m_engineVersion(engineVersion)
{
	m_loader = std::make_unique<Utils::bin::DynamicLibraryLoader>("vulkan-1");
	GET_PROC_ADDR(*m_loader, PFN_, vkCreateInstance);
	GET_PROC_ADDR(*m_loader, PFN_, vkDestroyInstance);
	GET_PROC_ADDR(*m_loader, PFN_, vkGetInstanceProcAddr);

	for (const auto& ae : additionalExtensions)
	{
		m_instanceExtensions.push_back(ae);
	}
#ifndef NDEBUG
	m_instanceExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

	m_instance = std::make_unique<Instance>(*this);

	VK_GET_INSTANCE_PROC_ADDR(m_instance->getHandle(), vkCreateDebugUtilsMessengerEXT);
	VK_GET_INSTANCE_PROC_ADDR(m_instance->getHandle(), vkDestroyDebugUtilsMessengerEXT);

	createDebugMessenger();
}

Context::~Context()
{
	destroyDebugMessenger();
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

	VkDebugUtilsMessengerEXT handle;
	if (vkCreateDebugUtilsMessengerEXT(m_instance->getHandle(), &createInfo, nullptr, &handle) != VK_SUCCESS)
		throw std::runtime_error("Failed to set up debug messenger");
	m_debugMessenger = std::make_unique<VkDebugUtilsMessengerEXT>(handle);
}
void Context::destroyDebugMessenger()
{
	vkDestroyDebugUtilsMessengerEXT(m_instance->getHandle(), *m_debugMessenger, nullptr);
}