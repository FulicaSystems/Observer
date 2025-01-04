#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include "binary/dynamic_library_loader.hpp"

class Context;
class PhysicalDevice;

class Surface;
class SwapChain;

#define VK_GET_DEVICE_PROC_ADDR(device, funcName) funcName = (PFN_##funcName)cx.vkGetDeviceProcAddr(device, #funcName)

class LogicalDevice
{
  private:
    const Context &cx;
    const PhysicalDevice &physicalHandle;

  private:
    // TODO : abstraction handle
    VkDevice m_handle;

    std::vector<const char *> m_deviceExtensions;

    void loadDeviceFunctions();

    void retrieveQueues();
    void createCommandPools();

  public:
    VkQueue graphicsQueue = nullptr;
    VkQueue presentQueue = nullptr;
#ifdef ENABLE_VIDEO_TRANSCODE
    VkQueue decodeQueue = nullptr;
    VkQueue encodeQueue = nullptr;
#endif

    // reset command pool
    VkCommandPool commandPool;
    // transient command pool
    VkCommandPool commandPoolTransient;
#ifdef ENABLE_VIDEO_TRANSCODE
    // decode reset command pool
    VkCommandPool commandPoolDecode;
    // encode reset command pool
    VkCommandPool commandPoolEncode;
#endif

    LogicalDevice() = delete;

    LogicalDevice(const LogicalDevice &copy) = delete;
    LogicalDevice &operator=(const LogicalDevice &copy) = delete;
    LogicalDevice(LogicalDevice &&move) = delete;
    LogicalDevice &operator=(LogicalDevice &&move) = delete;

    LogicalDevice(const Context &cx, const PhysicalDevice &physicalDevice) : cx(cx), physicalHandle(physicalDevice)
    {
    }
    ~LogicalDevice();

    void readyUp();

    /**
     * create a swapchain for a specified surface
     */
    [[nodiscard]] std::unique_ptr<SwapChain> createSwapChain(const Surface *presentationSurface) const;

    // template<class TDataType>
    // std::shared_ptr<TDataType> create(const void* createInfo) const { throw std::runtime_error("Use template
    // specialization"); } template<class TDataType> void destroy(std::shared_ptr<TDataType>& pData) const { throw
    // std::runtime_error("Use template specialization"); }

    // template<>
    // std::shared_ptr<class Buffer> create<class Buffer>(const void* createInfo) const;
    // template<>
    // void destroy<class Buffer>(std::shared_ptr<class Buffer>& pData) const;

    // template<>
    // std::shared_ptr<class ShaderModule> create<class ShaderModule>(const void* createInfo) const;
    // template<>
    // void destroy<class ShaderModule>(std::shared_ptr<class ShaderModule>& pData) const;

    // TODO : create<Image>

  public:
    [[nodiscard]] inline VkDevice &getHandle()
    {
        return m_handle;
    }
    [[nodiscard]] inline const VkDevice &getHandle() const
    {
        return m_handle;
    }

  public:
    // device utils functions
    PFN_DECLARE(PFN_, vkGetDeviceQueue);
    PFN_DECLARE(PFN_, vkDestroyDevice);

    PFN_DECLARE(PFN_, vkCreateCommandPool);
    PFN_DECLARE(PFN_, vkDestroyCommandPool);

    // swapchain functions
    PFN_DECLARE(PFN_, vkCreateSwapchainKHR);
    PFN_DECLARE(PFN_, vkGetSwapchainImagesKHR);
    PFN_DECLARE(PFN_, vkDestroyImageView);
    PFN_DECLARE(PFN_, vkDestroySwapchainKHR);

} typedef Device;
