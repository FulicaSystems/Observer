#pragma once

#include <iostream>

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
	GET_PROC_ADDR(*m_loader, PFN_, vkEnumerateInstanceLayerProperties);
	GET_PROC_ADDR(*m_loader, PFN_, vkEnumerateInstanceExtensionProperties);

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

	enumerateAvailableInstanceLayers();
	enumerateAvailableInstanceExtensions();
}

void Context::addLayer(const char* layer)
{
	m_layers.push_back(layer);
}
void Context::addInstanceExtension(const char* extension)
{
	m_instanceExtensions.push_back(extension);
}

void Context::enumerateAvailableInstanceLayers()
{
	uint32_t layerCount = 0;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	std::vector<VkLayerProperties> layers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, layers.data());
	std::cout << "available layers : " << layerCount << '\n';
	for (const auto& layer : layers)
		std::cout << '\t' << layer.layerName << '\n';
}
void Context::enumerateAvailableInstanceExtensions()
{
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
	std::cout << "available instance extensions : " << extensionCount << '\n';
	for (const auto& extension : extensions)
		std::cout << '\t' << extension.extensionName << '\n';
}
