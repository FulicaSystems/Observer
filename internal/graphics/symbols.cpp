#pragma once

#include "context.hpp"
#include "instance.hpp"

#include "symbols.hpp"

void InstanceSymbolsT::load(Utils::bin::DynamicLibraryLoader *loader)
{
    GET_PROC_ADDR(*loader, PFN_, vkCreateInstance);
    GET_PROC_ADDR(*loader, PFN_, vkDestroyInstance);
    GET_PROC_ADDR(*loader, PFN_, vkGetInstanceProcAddr);
    GET_PROC_ADDR(*loader, PFN_, vkEnumerateInstanceLayerProperties);
    GET_PROC_ADDR(*loader, PFN_, vkEnumerateInstanceExtensionProperties);

    GET_PROC_ADDR(*loader, PFN_, vkGetPhysicalDeviceProperties);
    GET_PROC_ADDR(*loader, PFN_, vkGetDeviceProcAddr);
}

void InstanceSymbols2T::load(const Context* cx, const Instance *instance)
{
    VK_GET_INSTANCE_PROC_ADDR(cx, instance->getHandle(), vkCreateDebugUtilsMessengerEXT);
    VK_GET_INSTANCE_PROC_ADDR(cx, instance->getHandle(), vkDestroyDebugUtilsMessengerEXT);

    VK_GET_INSTANCE_PROC_ADDR(cx, instance->getHandle(), vkEnumeratePhysicalDevices);

    VK_GET_INSTANCE_PROC_ADDR(cx, instance->getHandle(), vkDestroySurfaceKHR);
}

void DeviceSymbolsT::load(Utils::bin::DynamicLibraryLoader *loader)
{
    GET_PROC_ADDR(*loader, PFN_, vkEnumerateDeviceExtensionProperties);

    GET_PROC_ADDR(*loader, PFN_, vkGetPhysicalDeviceQueueFamilyProperties);
    GET_PROC_ADDR(*loader, PFN_, vkCreateDevice);
    GET_PROC_ADDR(*loader, PFN_, vkGetPhysicalDeviceSurfaceSupportKHR);

    GET_PROC_ADDR(*loader, PFN_, vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
    GET_PROC_ADDR(*loader, PFN_, vkGetPhysicalDeviceSurfaceFormatsKHR);
    GET_PROC_ADDR(*loader, PFN_, vkGetPhysicalDeviceSurfacePresentModesKHR);
}
