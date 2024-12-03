#pragma once

#include <memory>
#include <vector>

#include <vulkan/vulkan.h>

class Context;

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

    VkPhysicalDeviceProperties properties;
    VkPhysicalDeviceLimits limits;

    std::vector<VkQueueFamilyProperties> queueFamilies;

  public:
    PhysicalDevice() = delete;
    PhysicalDevice(const PhysicalDevice &copy) = delete;
    PhysicalDevice &operator=(const PhysicalDevice &copy) = delete;
    PhysicalDevice(PhysicalDevice &&) = delete;
    PhysicalDevice &operator=(PhysicalDevice &&) = delete;

    PhysicalDevice(const Context &cx, const char *deviceName);

    // std::unique_ptr<LogicalDevice> createDevice(const VkSurfaceKHR* presentationSurface = nullptr)

    // std::optional<uint32_t> findQueueFamilyIndex(const VkQueueFlags& capabilities) const
    // std::optional<uint32_t> findPresentQueueFamilyIndex(const VkSurfaceKHR& surface) const

    // class SwapchainSupport querySwapchainSupport(const VkSurfaceKHR& surface) const
};