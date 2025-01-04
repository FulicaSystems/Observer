#pragma once

#include <memory>
#include <optional>

#include <vulkan/vulkan.h>

#include "binary/dynamic_library_loader.hpp"

class Context;
class LogicalDevice;
class Surface;

// TODO : use this instead of raw Vulkan handle
struct PhysicalDeviceHandleT
{
    VkPhysicalDevice handle;
};

class PhysicalDevice
{
  public:
    static constexpr const uint32_t deviceExtensionCount = 1;
    static constexpr const char *deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

  private:
    const Context &cx;

  private:
    char deviceName[256];

    std::shared_ptr<VkPhysicalDevice> m_handle;

    VkPhysicalDeviceProperties m_properties;
    VkPhysicalDeviceLimits m_limits;

    std::vector<VkQueueFamilyProperties> m_queueFamilies;

  public:
    PhysicalDevice() = delete;
    PhysicalDevice(const PhysicalDevice &copy) = delete;
    PhysicalDevice &operator=(const PhysicalDevice &copy) = delete;
    PhysicalDevice(PhysicalDevice &&) = delete;
    PhysicalDevice &operator=(PhysicalDevice &&) = delete;

    PhysicalDevice(const Context &cx, const char *deviceName);

    std::unique_ptr<LogicalDevice> createDevice(const Surface *presentationSurface = nullptr);

    std::optional<uint32_t> findQueueFamilyIndex(const VkQueueFlags &capabilities) const;
    std::optional<uint32_t> findPresentQueueFamilyIndex(const Surface* surface) const;

    // class SwapchainSupport querySwapchainSupport(const VkSurfaceKHR& surface) const;
};