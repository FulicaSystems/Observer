#include "context.hpp"

#include "instance.hpp"

Instance::Instance(const Context& cx)
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

		if (cx.vkCreateInstance(&createInfo, nullptr, &m_handle) != VK_SUCCESS)
			throw std::runtime_error("Failed to create Vulkan instance");

}