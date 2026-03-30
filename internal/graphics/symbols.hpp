#pragma once

#include <vulkan/vulkan.h>

#include <binary/dynamic_library_loader.hpp>

class Context;
class Instance;

#define VK_GET_INSTANCE_PROC_ADDR(contextPtr, instance, funcName)                                                                  \
    funcName = (PFN_##funcName)contextPtr->vkGetInstanceProcAddr(instance, #funcName)

/**
 * @brief used to load api functions
 *
 */
// TODO : pass argument as const type*
struct SymbolsI
{
  protected:
    virtual void load(Utils::bin::DynamicLibraryLoader *loader) = 0;
    public:
    virtual void load(const Context* cx, const Instance* instance) = 0;
};

/**
 * @brief before instance creation
 *
 */
struct InstanceSymbolsT : public SymbolsI
{

    PFN_DECLARE(PFN_, vkCreateInstance);
    PFN_DECLARE(PFN_, vkDestroyInstance);
    PFN_DECLARE(PFN_, vkGetInstanceProcAddr);

    PFN_DECLARE(PFN_, vkEnumerateInstanceLayerProperties);
    PFN_DECLARE(PFN_, vkEnumerateInstanceExtensionProperties);

    PFN_DECLARE(PFN_, vkGetPhysicalDeviceProperties);

    PFN_DECLARE(PFN_, vkGetDeviceProcAddr);

  protected:
    void load(Utils::bin::DynamicLibraryLoader *loader) override;
    public:
    void load(const Context* cx, const Instance* instance) override{}
};

/**
 * @brief after instance creation
 *
 */
struct InstanceSymbols2T : public SymbolsI
{
    PFN_DECLARE(PFN_, vkCreateDebugUtilsMessengerEXT);
    PFN_DECLARE(PFN_, vkDestroyDebugUtilsMessengerEXT);
    PFN_DECLARE(PFN_, vkEnumeratePhysicalDevices);

    PFN_DECLARE(PFN_, vkDestroySurfaceKHR);

  protected:
    void load(Utils::bin::DynamicLibraryLoader *loader) override{}
    public:
    void load(const Context* cx, const Instance* instance) override;
};

/**
 * @brief before device creation
 *
 */
struct DeviceSymbolsT : public SymbolsI
{

    PFN_DECLARE(PFN_, vkEnumerateDeviceExtensionProperties);

    PFN_DECLARE(PFN_, vkGetPhysicalDeviceQueueFamilyProperties);
    PFN_DECLARE(PFN_, vkCreateDevice);
    PFN_DECLARE(PFN_, vkGetPhysicalDeviceSurfaceSupportKHR);

    PFN_DECLARE(PFN_, vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
    PFN_DECLARE(PFN_, vkGetPhysicalDeviceSurfaceFormatsKHR);
    PFN_DECLARE(PFN_, vkGetPhysicalDeviceSurfacePresentModesKHR);

  protected:
    void load(Utils::bin::DynamicLibraryLoader *loader) override;
    public:
    void load(const Context* cx, const Instance* instance) override{}
};
