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
  private:
    const Context &cx;

  private:
    char deviceName[256];

    std::shared_ptr<VkPhysicalDevice> m_handle;

    VkPhysicalDeviceProperties m_properties;
    VkPhysicalDeviceLimits m_limits;

    std::vector<VkQueueFamilyProperties> m_queueFamilies;

    std::optional<uint32_t> m_graphicsFamilyIndex;
    std::optional<uint32_t> m_presentFamilyIndex;
#ifdef ENABLE_VIDEO_TRANSCODE
    std::optional<uint32_t> m_decodeFamilyIndex;
    std::optional<uint32_t> m_encodeFamilyIndex;
#endif

  public:
    PhysicalDevice() = delete;
    PhysicalDevice(const PhysicalDevice &copy) = delete;
    PhysicalDevice &operator=(const PhysicalDevice &copy) = delete;
    PhysicalDevice(PhysicalDevice &&) = delete;
    PhysicalDevice &operator=(PhysicalDevice &&) = delete;

    PhysicalDevice(const Context &cx, const char *deviceName);

    std::vector<std::string> enumerateAvailableDeviceExtensions(const bool bDump = true) const;

    std::unique_ptr<LogicalDevice> createDevice(const Surface *presentationSurface = nullptr);

    std::optional<uint32_t> findQueueFamilyIndex(const VkQueueFlags &capabilities) const;
    std::optional<uint32_t> findPresentQueueFamilyIndex(const Surface *surface) const;

    // class SwapchainSupport querySwapchainSupport(const VkSurfaceKHR& surface) const;

  public:
    inline std::optional<uint32_t> getGraphicsFamilyIndex() const
    {
        return m_graphicsFamilyIndex;
    }
    inline std::optional<uint32_t> getPresentFamilyIndex() const
    {
        return m_presentFamilyIndex;
    }
#ifdef ENABLE_VIDEO_TRANSCODE
    inline std::optional<uint32_t> getDecodeFamilyIndex() const
    {
        return m_decodeFamilyIndex;
    }
    inline std::optional<uint32_t> getEncodeFamilyIndex() const
    {
        return m_encodeFamilyIndex;
    }
#endif
};