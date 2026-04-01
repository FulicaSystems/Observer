#pragma once

#include <memory>
#include <optional>

#include <vulkan/vulkan.h>

class ContextABC;
class Instance;
class LogicalDevice;

class Surface;
struct SurfaceDetailsT;

// TODO : use this instead of raw Vulkan handle
struct PhysicalDeviceHandleT
{
    VkPhysicalDevice handle;
};

struct PhysicalDeviceCreateInfoT
{
    ContextABC* cx;
    const Instance* inst;
    const char* deviceName;
    Surface* surface;
};

class PhysicalDevice
{
  private:
    ContextABC* cx;
    const Instance* inst;

  private:
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

    void initPhysicalDeviceProperties();
    void initQueueFamilyProperties();
    /**
     * specify a surface if one wants this physical device to support presentation
     */
    void initQueueFamilyIndices(const Surface* presentationSurface = nullptr);

  public:
    PhysicalDevice() = delete;
    PhysicalDevice(const PhysicalDevice& copy) = delete;
    PhysicalDevice& operator=(const PhysicalDevice& copy) = delete;
    PhysicalDevice(PhysicalDevice&&) = delete;
    PhysicalDevice& operator=(PhysicalDevice&&) = delete;

    PhysicalDevice(const PhysicalDeviceCreateInfoT createInfo);

    std::vector<std::string> enumerateAvailableDeviceExtensions(const bool bDump = true) const;

    [[nodiscard]] std::unique_ptr<LogicalDevice> createDevice() const;

    [[nodiscard]] std::optional<uint32_t> findQueueFamilyIndex(
        const VkQueueFlags& capabilities) const;
    [[nodiscard]] std::optional<uint32_t> findPresentQueueFamilyIndex(const Surface* surface) const;

    [[nodiscard]] VkSurfaceCapabilitiesKHR getSurfaceCapabilities(const Surface& surface) const;
    [[deprecated, nodiscard]] SurfaceDetailsT getSurfaceDetails(const Surface& surface) const;

  public:
    [[nodiscard]] inline VkPhysicalDevice getHandle() const { return *m_handle; }
    [[nodiscard]] inline const Instance* getInstance() const { return inst; }

    [[nodiscard]] inline std::optional<uint32_t> getGraphicsFamilyIndex() const
    {
        return m_graphicsFamilyIndex;
    }
    [[nodiscard]] inline std::optional<uint32_t> getPresentFamilyIndex() const
    {
        return m_presentFamilyIndex;
    }
#ifdef ENABLE_VIDEO_TRANSCODE
    [[nodiscard]] inline std::optional<uint32_t> getDecodeFamilyIndex() const
    {
        return m_decodeFamilyIndex;
    }
    [[nodiscard]] inline std::optional<uint32_t> getEncodeFamilyIndex() const
    {
        return m_encodeFamilyIndex;
    }
#endif

    [[nodiscard]] VkPhysicalDeviceType getDeviceType() const { return m_properties.deviceType; }

    [[nodiscard]] const char* getDeviceName() const { return m_properties.deviceName; }
};