#pragma once

#include <vector>
#include <memory>
#include <string>

#include <vulkan/vulkan.h>

#include <binary/dynamic_library_loader.hpp>

#include "instance.hpp"


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

#define VK_GET_INSTANCE_PROC_ADDR(instance, funcName) funcName = (PFN_##funcName)vkGetInstanceProcAddr(instance, #funcName)

class Context
{
private:
	std::string m_applicationName;
	version m_applicationVersion;
	version m_engineVersion;

	std::vector<const char*> m_layers;
	std::vector<const char*> m_instanceExtensions;

	std::unique_ptr<Utils::bin::DynamicLibraryLoader> m_loader;

	std::unique_ptr<Instance> m_instance;

	//std::unique_ptr<class DeviceSelector> deviceSelector = nullptr;

public:
    Context() = delete;
	Context(const char* applicationName,
		const version applicationVersion,
		const version engineVersion,
		std::vector<const char*> additionalExtensions);

	void addLayer(const char* layer);
	void addInstanceExtension(const char* extension);

	std::vector<const char*> enumerateAvailableInstanceLayers(const bool bDump = true);
	std::vector<const char*> enumerateAvailableInstanceExtensions(const bool bDump = true);
	std::vector<VkPhysicalDevice> enumerateAvailablePhysicalDevices(const bool bDump = true);

public:
	inline const std::string& getApplicationName() const
	{ return m_applicationName; }
	inline const version getApplicationVersion() const
	{ return m_applicationVersion; }
	inline const version getEngineVersion() const
	{ return m_engineVersion; }
	inline const std::vector<const char*> getLayers() const
	{ return m_layers; }
	inline const std::vector<const char*> getInstanceExtensions() const
	{ return m_instanceExtensions; }

public:
	// base functions
	PFN_DECLARE(PFN_, vkCreateInstance);
	PFN_DECLARE(PFN_, vkDestroyInstance);
	PFN_DECLARE(PFN_, vkGetInstanceProcAddr);
	PFN_DECLARE(PFN_, vkEnumerateInstanceLayerProperties);
	PFN_DECLARE(PFN_, vkEnumerateInstanceExtensionProperties);

	PFN_DECLARE(PFN_, vkGetPhysicalDeviceProperties);

	// instance
	PFN_DECLARE(PFN_, vkCreateDebugUtilsMessengerEXT);
	PFN_DECLARE(PFN_, vkDestroyDebugUtilsMessengerEXT);
	PFN_DECLARE(PFN_, vkEnumeratePhysicalDevices);
};