#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <vulkan/vulkan.h>

#include <binary/dynamic_library_loader.hpp>

#include "instance.hpp"
#include "symbols.hpp"

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

/**
 * @brief class used to specify options in order to create a context object
 *
 */
struct ContextCreateInfoT
{
    const char *applicationName;
    version applicationVersion;
    version engineVersion;

    std::vector<const char *> layers;
    std::vector<const char *> instanceExtensions;
    std::vector<const char *> deviceExtensions;
};

/**
 * Context object contains symbols loader and instance creation utils
 * this object is used to call api functions
 */
class Context : public InstanceSymbolsT,
                public InstanceSymbols2T,
                public DeviceSymbolsT,
                public DeviceSymbols2T,
                public SwapchainSymbolsT
{
  private:
    std::string m_applicationName;
    version m_applicationVersion;
    version m_engineVersion;

    std::vector<const char *> m_layers;
    std::vector<const char *> m_instanceExtensions;
    std::vector<const char *> m_deviceExtensions;

    std::unique_ptr<Utils::bin::DynamicLibraryLoader> m_loader;

  public:
    Context() = delete;
    Context(const Context &) = delete;
    Context &operator=(const Context &) = delete;
    Context(Context &&) = delete;
    Context &operator=(Context &&) = delete;

    Context(const ContextCreateInfoT createInfo);

    /**
     * returns an array with all the instance layer names
     */
    std::vector<std::string> enumerateAvailableInstanceLayers(const bool bDump = true) const;
    /**
     * returns an array with all the instance extension names
     */
    std::vector<std::string> enumerateAvailableInstanceExtensions(const bool bDump = true) const;

  public:
    inline const std::string &getApplicationName() const
    {
        return m_applicationName;
    }
    inline const version getApplicationVersion() const
    {
        return m_applicationVersion;
    }
    inline const version getEngineVersion() const
    {
        return m_engineVersion;
    }
    inline const std::vector<const char *> getLayers() const
    {
        return m_layers;
    }
    inline const std::vector<const char *> getInstanceExtensions() const
    {
        return m_instanceExtensions;
    }
    inline const std::vector<const char *> getDeviceExtensions() const
    {
        return m_deviceExtensions;
    }
};