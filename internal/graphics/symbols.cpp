#pragma once

#include "context.hpp"
#include "device/device.hpp"
#include "instance.hpp"

#include "symbols.hpp"

void InstanceSymbolsLoaderT::load(ContextABC* cx, f6::bin::DynamicLibraryLoader* loader)
{
    cx->GET_PROC_ADDR(*loader, PFN_vk, vk, CreateInstance);
    cx->GET_PROC_ADDR(*loader, PFN_vk, vk, DestroyInstance);
    cx->GET_PROC_ADDR(*loader, PFN_vk, vk, GetInstanceProcAddr);
    cx->GET_PROC_ADDR(*loader, PFN_vk, vk, EnumerateInstanceLayerProperties);
    cx->GET_PROC_ADDR(*loader, PFN_vk, vk, EnumerateInstanceExtensionProperties);

    cx->GET_PROC_ADDR(*loader, PFN_vk, vk, GetPhysicalDeviceProperties);
    cx->GET_PROC_ADDR(*loader, PFN_vk, vk, GetDeviceProcAddr);
}

void InstanceSymbolsLoader2T::load(ContextABC* cx, const Instance* instance)
{
    VK_GET_INSTANCE_PROC_ADDR(cx, instance->getHandle(), CreateDebugUtilsMessengerEXT);
    VK_GET_INSTANCE_PROC_ADDR(cx, instance->getHandle(), DestroyDebugUtilsMessengerEXT);

    VK_GET_INSTANCE_PROC_ADDR(cx, instance->getHandle(), EnumeratePhysicalDevices);

    VK_GET_INSTANCE_PROC_ADDR(cx, instance->getHandle(), DestroySurfaceKHR);
}

void DeviceSymbolsLoaderT::load(ContextABC* cx, f6::bin::DynamicLibraryLoader* loader)
{
    cx->GET_PROC_ADDR(*loader, PFN_vk, vk, EnumerateDeviceExtensionProperties);

    cx->GET_PROC_ADDR(*loader, PFN_vk, vk, GetPhysicalDeviceQueueFamilyProperties);
    cx->GET_PROC_ADDR(*loader, PFN_vk, vk, CreateDevice);
    cx->GET_PROC_ADDR(*loader, PFN_vk, vk, GetPhysicalDeviceSurfaceSupportKHR);

    cx->GET_PROC_ADDR(*loader, PFN_vk, vk, GetPhysicalDeviceSurfaceCapabilitiesKHR);
    cx->GET_PROC_ADDR(*loader, PFN_vk, vk, GetPhysicalDeviceSurfaceFormatsKHR);
    cx->GET_PROC_ADDR(*loader, PFN_vk, vk, GetPhysicalDeviceSurfacePresentModesKHR);
}

void DeviceSymbolsLoader2T::load(ContextABC* cx, const LogicalDevice* device)
{
    VK_GET_DEVICE_PROC_ADDR(cx, device->getHandle(), GetDeviceQueue);
    VK_GET_DEVICE_PROC_ADDR(cx, device->getHandle(), DestroyDevice);

    VK_GET_DEVICE_PROC_ADDR(cx, device->getHandle(), CreateCommandPool);
    VK_GET_DEVICE_PROC_ADDR(cx, device->getHandle(), DestroyCommandPool);

    SwapchainSymbolsLoaderT::load(cx, device);
    BufferSymbolsLoaderT::load(cx, device);
    ImageSymbolsLoaderT::load(cx, device);
    RenderPassSymbolsLoaderT::load(cx, device);
    PipelineSymbolsLoaderT::load(cx, device);
    BackBufferSymbolsLoaderT::load(cx, device);
    RenderingSymbolsLoaderT::load(cx, device);
}

void SwapchainSymbolsLoaderT::load(ContextABC* cx, const LogicalDevice* device)
{
    VK_GET_DEVICE_PROC_ADDR(cx, device->getHandle(), CreateSwapchainKHR);
    VK_GET_DEVICE_PROC_ADDR(cx, device->getHandle(), GetSwapchainImagesKHR);
    VK_GET_DEVICE_PROC_ADDR(cx, device->getHandle(), DestroySwapchainKHR);
}

void SDKSymbolsLoaderT::load(ContextABC* cx)
{
    VK_SDK_FUNCTION(cx, CreateInstance);
    VK_SDK_FUNCTION(cx, DestroyInstance);
    VK_SDK_FUNCTION(cx, GetInstanceProcAddr);
    VK_SDK_FUNCTION(cx, EnumerateInstanceLayerProperties);
    VK_SDK_FUNCTION(cx, EnumerateInstanceExtensionProperties);
    VK_SDK_FUNCTION(cx, GetPhysicalDeviceProperties);
    VK_SDK_FUNCTION(cx, GetDeviceProcAddr);
    VK_SDK_FUNCTION(cx, EnumeratePhysicalDevices);
    VK_SDK_FUNCTION(cx, DestroySurfaceKHR);
    VK_SDK_FUNCTION(cx, EnumerateDeviceExtensionProperties);
    VK_SDK_FUNCTION(cx, GetPhysicalDeviceQueueFamilyProperties);
    VK_SDK_FUNCTION(cx, CreateDevice);
    VK_SDK_FUNCTION(cx, GetPhysicalDeviceSurfaceSupportKHR);
    VK_SDK_FUNCTION(cx, GetPhysicalDeviceSurfaceCapabilitiesKHR);
    VK_SDK_FUNCTION(cx, GetPhysicalDeviceSurfaceFormatsKHR);
    VK_SDK_FUNCTION(cx, GetPhysicalDeviceSurfacePresentModesKHR);
    VK_SDK_FUNCTION(cx, GetDeviceQueue);
    VK_SDK_FUNCTION(cx, DestroyDevice);
    VK_SDK_FUNCTION(cx, CreateCommandPool);
    VK_SDK_FUNCTION(cx, DestroyCommandPool);
    VK_SDK_FUNCTION(cx, CreateSwapchainKHR);
    VK_SDK_FUNCTION(cx, GetSwapchainImagesKHR);
    VK_SDK_FUNCTION(cx, DestroySwapchainKHR);
    VK_SDK_FUNCTION(cx, CreateImageView);
    VK_SDK_FUNCTION(cx, DestroyImageView);
    VK_SDK_FUNCTION(cx, CreateRenderPass);
    VK_SDK_FUNCTION(cx, DestroyRenderPass);
    VK_SDK_FUNCTION(cx, CreateShaderModule);
    VK_SDK_FUNCTION(cx, DestroyShaderModule);
    VK_SDK_FUNCTION(cx, CreateDescriptorSetLayout);
    VK_SDK_FUNCTION(cx, CreatePipelineLayout);
    VK_SDK_FUNCTION(cx, DestroyPipelineLayout);
    VK_SDK_FUNCTION(cx, CreateGraphicsPipelines);
    VK_SDK_FUNCTION(cx, DestroyPipeline);
    VK_SDK_FUNCTION(cx, AllocateCommandBuffers);
    VK_SDK_FUNCTION(cx, CreateSemaphore);
    VK_SDK_FUNCTION(cx, CreateFence);
    VK_SDK_FUNCTION(cx, CreateBuffer);
    VK_SDK_FUNCTION(cx, DestroyBuffer);
    VK_SDK_FUNCTION(cx, CreateFramebuffer);
    VK_SDK_FUNCTION(cx, DestroyFramebuffer);
    VK_SDK_FUNCTION(cx, WaitForFences);
    VK_SDK_FUNCTION(cx, ResetFences);
    VK_SDK_FUNCTION(cx, AcquireNextImageKHR);
}

void SDKSymbolsLoaderT::load(ContextABC* cx, const Instance* instance)
{
    VK_GET_INSTANCE_PROC_ADDR(cx, instance->getHandle(), CreateDebugUtilsMessengerEXT);
    VK_GET_INSTANCE_PROC_ADDR(cx, instance->getHandle(), DestroyDebugUtilsMessengerEXT);
}

void ImageSymbolsLoaderT::load(ContextABC* cx, const LogicalDevice* device)
{
    VK_GET_DEVICE_PROC_ADDR(cx, device->getHandle(), CreateImageView);
    VK_GET_DEVICE_PROC_ADDR(cx, device->getHandle(), DestroyImageView);
}

void RenderPassSymbolsLoaderT::load(ContextABC* cx, const LogicalDevice* device)
{
    VK_GET_DEVICE_PROC_ADDR(cx, device->getHandle(), CreateRenderPass);
    VK_GET_DEVICE_PROC_ADDR(cx, device->getHandle(), DestroyRenderPass);
}

void PipelineSymbolsLoaderT::load(ContextABC* cx, const LogicalDevice* device)
{
    VK_GET_DEVICE_PROC_ADDR(cx, device->getHandle(), CreateShaderModule);
    VK_GET_DEVICE_PROC_ADDR(cx, device->getHandle(), DestroyShaderModule);

    VK_GET_DEVICE_PROC_ADDR(cx, device->getHandle(), CreateDescriptorSetLayout);
    VK_GET_DEVICE_PROC_ADDR(cx, device->getHandle(), CreatePipelineLayout);
    VK_GET_DEVICE_PROC_ADDR(cx, device->getHandle(), DestroyPipelineLayout);
    VK_GET_DEVICE_PROC_ADDR(cx, device->getHandle(), CreateGraphicsPipelines);
    VK_GET_DEVICE_PROC_ADDR(cx, device->getHandle(), DestroyPipeline);
}

void BackBufferSymbolsLoaderT::load(ContextABC* cx, const LogicalDevice* device)
{
    VK_GET_DEVICE_PROC_ADDR(cx, device->getHandle(), AllocateCommandBuffers);
    VK_GET_DEVICE_PROC_ADDR(cx, device->getHandle(), CreateSemaphore);
    VK_GET_DEVICE_PROC_ADDR(cx, device->getHandle(), CreateFence);
}

void BufferSymbolsLoaderT::load(ContextABC* cx, const LogicalDevice* device)
{
    VK_GET_DEVICE_PROC_ADDR(cx, device->getHandle(), CreateBuffer);
    VK_GET_DEVICE_PROC_ADDR(cx, device->getHandle(), DestroyBuffer);
}

void RenderingSymbolsLoaderT::load(ContextABC* cx, const LogicalDevice* device)
{
    VK_GET_DEVICE_PROC_ADDR(cx, device->getHandle(), WaitForFences);
    VK_GET_DEVICE_PROC_ADDR(cx, device->getHandle(), ResetFences);
    VK_GET_DEVICE_PROC_ADDR(cx, device->getHandle(), AcquireNextImageKHR);
}
