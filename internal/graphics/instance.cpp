#include "context.hpp"

#include "instance.hpp"

Instance::Instance(const Context& cx)
	: cx(cx)
{
	version appv = cx.getApplicationVersion();
	version engv = cx.getEngineVersion();

	VkApplicationInfo appInfo = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = cx.getApplicationName().c_str(),
		.applicationVersion = VK_MAKE_API_VERSION(0,
			MAJOR(appv),
			MINOR(appv),
			PATCH(appv)),
		.engineVersion = VK_MAKE_API_VERSION(0,
			MAJOR(engv),
			MINOR(engv),
			PATCH(engv)),
		.apiVersion = VK_API_VERSION_1_3
	};

	auto layers = cx.getLayers();
	auto instanceExtensions = cx.getInstanceExtensions();
	VkInstanceCreateInfo createInfo = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo = &appInfo,
		.enabledLayerCount = static_cast<uint32_t>(layers.size()),
		.ppEnabledLayerNames = layers.data(),
		.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size()),
		.ppEnabledExtensionNames = instanceExtensions.data()
	};

	VkInstance handle;
	if (cx.vkCreateInstance(&createInfo, nullptr, &handle) != VK_SUCCESS)
		throw std::runtime_error("Failed to create Vulkan instance");

	m_handle = std::make_unique<VkInstance>(handle);

	//createDebugMessenger();
}

Instance::~Instance()
{
	if (m_debugMessenger)
		destroyDebugMessenger();
	cx.vkDestroyInstance(*m_handle, nullptr);
}

void Instance::createDebugMessenger()
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
	if (cx.vkCreateDebugUtilsMessengerEXT(*m_handle, &createInfo, nullptr, &handle) != VK_SUCCESS)
		throw std::runtime_error("Failed to set up debug messenger");
	m_debugMessenger = std::make_unique<VkDebugUtilsMessengerEXT>(handle);
}
void Instance::destroyDebugMessenger()
{
	cx.vkDestroyDebugUtilsMessengerEXT(*m_handle, *m_debugMessenger, nullptr);
}