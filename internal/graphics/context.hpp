#pragma once

#include <vulkan/vulkan.h>

// 32 bits
// 4 bits for major, 12 bits for minor, 16 bits for patch
// MMMM mmmm mmmm mmmm pppp pppp pppp pppp
#define VERSION(major, minor, patch) (uint32_t)(major << 28U | minor << 16U | patch << 0U)
#define MAJOR(version) (uint32_t)((version >> 28U) & 0xfU)
#define MINOR(version) (uint32_t)((version >> 16U) & 0x0fffU)
#define PATCH(version) (uint32_t)((version >> 0U) & 0xfU)
#ifdef VERSION_STRUCT
struct version
{
	uint32_t major : 4;
	uint32_t minor : 12;
	uint32_t patch : 16;
};
#else
typedef uint32_t version;
#endif

class Context
{
public:
	std::vector<const char*> m_layers;
	std::vector<const char*> m_instanceExtensions;

	std::unique_ptr<Utils::bin::DynamicLibraryLoader> m_loader;

	std::unique_ptr<Instance> m_instance;


	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
		void* userData)
	{
		std::cerr << "[Validation Layer] : " << callbackData->pMessage << std::endl;
		return VK_FALSE;
	}

	VkDebugUtilsMessengerEXT debugMessenger;

	std::unique_ptr<class DeviceSelector> deviceSelector = nullptr;

	Context(const char* applicationName,
		const version applicationVersion,
		const version engineVersion,
		std::vector<const char*> additionalExtensions);
	~Context();

	void createDebugMessenger();
	void destroyDebugMessenger();

	PFN_vkCreateInstance vkCreateInstance;
};