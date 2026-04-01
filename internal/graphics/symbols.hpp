#pragma once

#include <vulkan/vulkan.h>

#include <f6/dynamic_library_loader.hpp>

class ContextABC;
class Instance;
class LogicalDevice;

#define VK_GET_INSTANCE_PROC_ADDR(contextPtr, instance, funcName)                   \
    contextPtr->funcName =                                                          \
        (PFN_vk##funcName)contextPtr->GetInstanceProcAddr(instance, "vk" #funcName)

#define VK_GET_DEVICE_PROC_ADDR(contextPtr, device, funcName)                                      \
    contextPtr->funcName = (PFN_vk##funcName)contextPtr->GetDeviceProcAddr(device, "vk" #funcName)

#define VK_SDK_FUNCTION(contextPtr, funcName) contextPtr->funcName = &vk##funcName

/**
 * @brief used to load api functions
 *
 */
// TODO : pass argument as const type*
struct SymbolsLoaderI
{
  protected:
    virtual void load(ContextABC* cx) = 0;
    virtual void load(ContextABC* cx, f6::bin::DynamicLibraryLoader* loader) = 0;

    virtual void load(ContextABC* cx, const Instance* instance) = 0;
    virtual void load(ContextABC* cx, const LogicalDevice* device) = 0;
};

struct SDKSymbolsLoaderT : public SymbolsLoaderI
{
  protected:
    void load(ContextABC* cx) override;
    void load(ContextABC* cx, f6::bin::DynamicLibraryLoader* loader) override {};

    void load(ContextABC* cx, const Instance* instance) override;
    void load(ContextABC* cx, const LogicalDevice* device) override {};
};

/**
 * @brief before instance creation
 *
 */
struct InstanceSymbolsT
{
    PFN_DECLARE(PFN_vk, CreateInstance);
    PFN_DECLARE(PFN_vk, DestroyInstance);
    PFN_DECLARE(PFN_vk, GetInstanceProcAddr);

    PFN_DECLARE(PFN_vk, EnumerateInstanceLayerProperties);
    PFN_DECLARE(PFN_vk, EnumerateInstanceExtensionProperties);

    PFN_DECLARE(PFN_vk, GetPhysicalDeviceProperties);

    PFN_DECLARE(PFN_vk, GetDeviceProcAddr);
};

struct InstanceSymbolsLoaderT : public SymbolsLoaderI
{
  protected:
    void load(ContextABC* cx) override {};
    void load(ContextABC* cx, f6::bin::DynamicLibraryLoader* loader) override;

    void load(ContextABC* cx, const Instance* instance) override {}
    void load(ContextABC* cx, const LogicalDevice* device) override {}
};

/**
 * @brief after instance creation
 *
 */
struct InstanceSymbols2T
{
    PFN_DECLARE(PFN_vk, CreateDebugUtilsMessengerEXT);
    PFN_DECLARE(PFN_vk, DestroyDebugUtilsMessengerEXT);
    PFN_DECLARE(PFN_vk, EnumeratePhysicalDevices);

    PFN_DECLARE(PFN_vk, DestroySurfaceKHR);
};
struct InstanceSymbolsLoader2T : public SymbolsLoaderI
{
  protected:
    void load(ContextABC* cx) override {};
    void load(ContextABC* cx, f6::bin::DynamicLibraryLoader* loader) override {}

    void load(ContextABC* cx, const Instance* instance) override;
    void load(ContextABC* cx, const LogicalDevice* device) override {}
};

/**
 * @brief before device creation
 *
 */
struct DeviceSymbolsT
{
    PFN_DECLARE(PFN_vk, EnumerateDeviceExtensionProperties);

    PFN_DECLARE(PFN_vk, GetPhysicalDeviceQueueFamilyProperties);
    PFN_DECLARE(PFN_vk, CreateDevice);
    PFN_DECLARE(PFN_vk, GetPhysicalDeviceSurfaceSupportKHR);

    PFN_DECLARE(PFN_vk, GetPhysicalDeviceSurfaceCapabilitiesKHR);
    PFN_DECLARE(PFN_vk, GetPhysicalDeviceSurfaceFormatsKHR);
    PFN_DECLARE(PFN_vk, GetPhysicalDeviceSurfacePresentModesKHR);
};
struct DeviceSymbolsLoaderT : public SymbolsLoaderI
{
  protected:
    void load(ContextABC* cx) override {};
    void load(ContextABC* cx, f6::bin::DynamicLibraryLoader* loader) override;

    void load(ContextABC* cx, const Instance* instance) override {}
    void load(ContextABC* cx, const LogicalDevice* device) override {}
};

struct SwapchainSymbolsT
{
    PFN_DECLARE(PFN_vk, CreateSwapchainKHR);
    PFN_DECLARE(PFN_vk, DestroySwapchainKHR);
    PFN_DECLARE(PFN_vk, GetSwapchainImagesKHR);
};
struct SwapchainSymbolsLoaderT : public SymbolsLoaderI
{
  protected:
    void load(ContextABC* cx) override {};
    void load(ContextABC* cx, f6::bin::DynamicLibraryLoader* loader) override {}

    void load(ContextABC* cx, const Instance* instance) override {}
    void load(ContextABC* cx, const LogicalDevice* device) override;
};

struct BufferSymbolsT : public SwapchainSymbolsT
{
    PFN_DECLARE(PFN_vk, CreateBuffer);
    PFN_DECLARE(PFN_vk, DestroyBuffer);
};
struct BufferSymbolsLoaderT : public SwapchainSymbolsLoaderT
{
  protected:
    void load(ContextABC* cx) override {};
    void load(ContextABC* cx, f6::bin::DynamicLibraryLoader* loader) override {}

    void load(ContextABC* cx, const Instance* instance) override {}
    void load(ContextABC* cx, const LogicalDevice* device) override;
};

struct ImageSymbolsT : public BufferSymbolsT
{
    PFN_DECLARE(PFN_vk, CreateImageView);
    PFN_DECLARE(PFN_vk, DestroyImageView);
};
struct ImageSymbolsLoaderT : public BufferSymbolsLoaderT
{
  protected:
    void load(ContextABC* cx) override {};
    void load(ContextABC* cx, f6::bin::DynamicLibraryLoader* loader) override {}

    void load(ContextABC* cx, const Instance* instance) override {}
    void load(ContextABC* cx, const LogicalDevice* device) override;
};

struct RenderPassSymbolsT : public ImageSymbolsT
{
    PFN_DECLARE(PFN_vk, CreateRenderPass);
    PFN_DECLARE(PFN_vk, DestroyRenderPass);

    PFN_DECLARE(PFN_vk, CreateFramebuffer);
    PFN_DECLARE(PFN_vk, DestroyFramebuffer);
};
struct RenderPassSymbolsLoaderT : public ImageSymbolsLoaderT
{
  protected:
    void load(ContextABC* cx) override {};
    void load(ContextABC* cx, f6::bin::DynamicLibraryLoader* loader) override {}

    void load(ContextABC* cx, const Instance* instance) override {}
    void load(ContextABC* cx, const LogicalDevice* device) override;
};

struct PipelineSymbolsT : public RenderPassSymbolsT
{
    PFN_DECLARE(PFN_vk, CreateShaderModule);
    PFN_DECLARE(PFN_vk, DestroyShaderModule);

    PFN_DECLARE(PFN_vk, CreateDescriptorSetLayout);
    PFN_DECLARE(PFN_vk, CreatePipelineLayout);
    PFN_DECLARE(PFN_vk, DestroyPipelineLayout);
    PFN_DECLARE(PFN_vk, CreateGraphicsPipelines);
    PFN_DECLARE(PFN_vk, DestroyPipeline);
};
struct PipelineSymbolsLoaderT : public RenderPassSymbolsLoaderT
{
    void load(ContextABC* cx) override {};
    void load(ContextABC* cx, f6::bin::DynamicLibraryLoader* loader) override {}

    void load(ContextABC* cx, const Instance* instance) override {}
    void load(ContextABC* cx, const LogicalDevice* device) override;
};
struct BackBufferSymbolsT : public PipelineSymbolsT
{
    PFN_DECLARE(PFN_vk, AllocateCommandBuffers);
    PFN_DECLARE(PFN_vk, CreateSemaphore);
    PFN_DECLARE(PFN_vk, CreateFence);
};
struct BackBufferSymbolsLoaderT : public PipelineSymbolsLoaderT
{
    void load(ContextABC* cx) override {};
    void load(ContextABC* cx, f6::bin::DynamicLibraryLoader* loader) override {}

    void load(ContextABC* cx, const Instance* instance) override {}
    void load(ContextABC* cx, const LogicalDevice* device) override;
};

struct RenderingSymbolsT : public BackBufferSymbolsT
{
    PFN_DECLARE(PFN_vk, WaitForFences);
    PFN_DECLARE(PFN_vk, ResetFences);
    PFN_DECLARE(PFN_vk, AcquireNextImageKHR);
};
struct RenderingSymbolsLoaderT : public BackBufferSymbolsLoaderT
{
    void load(ContextABC* cx) override {};
    void load(ContextABC* cx, f6::bin::DynamicLibraryLoader* loader) override {}

    void load(ContextABC* cx, const Instance* instance) override {}
    void load(ContextABC* cx, const LogicalDevice* device) override;
};

struct DeviceSymbols2T : public RenderingSymbolsT
{
    PFN_DECLARE(PFN_vk, GetDeviceQueue);
    PFN_DECLARE(PFN_vk, DestroyDevice);

    PFN_DECLARE(PFN_vk, CreateCommandPool);
    PFN_DECLARE(PFN_vk, DestroyCommandPool);
};
struct DeviceSymbolsLoader2T : public RenderingSymbolsLoaderT
{
  protected:
    void load(ContextABC* cx) override {};
    void load(ContextABC* cx, f6::bin::DynamicLibraryLoader* loader) override {}

    void load(ContextABC* cx, const Instance* instance) override {}
    void load(ContextABC* cx, const LogicalDevice* device) override;
};
