#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <vulkan/vulkan.h>

#include <f6/dynamic_library_loader.hpp>

#include "symbols.hpp"

class Instance;
class LogicalDevice;

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
    const char* applicationName;
    version applicationVersion;
    version engineVersion;

    std::vector<const char*> layers;
    std::vector<const char*> instanceExtensions;
    std::vector<const char*> deviceExtensions;
};

class ContextABC : public InstanceSymbolsT,
                   public InstanceSymbols2T,
                   public DeviceSymbolsT,
                   public DeviceSymbols2T
{
  protected:
    std::string m_applicationName;
    version m_applicationVersion;
    version m_engineVersion;

    ContextCreateInfoT ci;

  public:
    ContextABC() = delete;
    ContextABC(const ContextABC&) = delete;
    ContextABC& operator=(const ContextABC&) = delete;
    ContextABC(ContextABC&&) = delete;
    ContextABC& operator=(ContextABC&&) = delete;

    explicit ContextABC(const ContextCreateInfoT createInfo);

    virtual ~ContextABC() {}

    /**
     * @brief load instance symbols
     *
     */
    virtual void loadTop(const Instance* inst) {};
    /**
     * @brief load device symbols
     *
     */
    virtual void loadBottom(const LogicalDevice* dev) {};

    /**
     * returns an array with all the instance layer names
     */
    std::vector<std::string> enumerateAvailableInstanceLayers(const bool bDump = true) const;
    /**
     * returns an array with all the instance extension names
     */
    std::vector<std::string> enumerateAvailableInstanceExtensions(const bool bDump = true) const;

  public:
    inline const std::string& getApplicationName() const { return m_applicationName; }
    inline const version getApplicationVersion() const { return m_applicationVersion; }
    inline const version getEngineVersion() const { return m_engineVersion; }
    inline const std::vector<const char*> getLayers() const { return ci.layers; }
    inline const std::vector<const char*> getInstanceExtensions() const
    {
        return ci.instanceExtensions;
    }
    inline const std::vector<const char*> getDeviceExtensions() const
    {
        return ci.deviceExtensions;
    }
};

/**
 * @brief context using the VulkanSDK as symbols loader
 *
 */
class ContextSDK : public ContextABC, public SDKSymbolsLoaderT
{
  public:
    ContextSDK() = delete;
    ContextSDK(const ContextSDK&) = delete;
    ContextSDK& operator=(const ContextSDK&) = delete;
    ContextSDK(ContextSDK&&) = delete;
    ContextSDK& operator=(ContextSDK&&) = delete;

    ContextSDK(const ContextCreateInfoT createInfo) : ContextABC(createInfo)
    {
        SDKSymbolsLoaderT::load(this);
    }

    void loadTop(const Instance* inst) override { SDKSymbolsLoaderT::load(this, inst); }
};

// TODO
class ContextGlad : public ContextABC
{
  public:
    ContextGlad() = delete;
    ContextGlad(const ContextGlad&) = delete;
    ContextGlad& operator=(const ContextGlad&) = delete;
    ContextGlad(ContextGlad&&) = delete;
    ContextGlad& operator=(ContextGlad&&) = delete;

    ContextGlad(const ContextCreateInfoT createInfo);
};

// TODO
class ContextVolk : public ContextABC
{
  public:
    ContextVolk() = delete;
    ContextVolk(const ContextVolk&) = delete;
    ContextVolk& operator=(const ContextVolk&) = delete;
    ContextVolk(ContextVolk&&) = delete;
    ContextVolk& operator=(ContextVolk&&) = delete;

    ContextVolk(const ContextCreateInfoT createInfo);
};

/**
 * Context object contains symbols loader and instance creation utils
 * this object is used to call api functions
 * this implementation allows the Context to use any loader (custom/glad/volk/vulkansdk)
 * context directly loading the symbols from the vulkan-1.dll in the OS folder
 */
class ContextDLL : public ContextABC,
                   public InstanceSymbolsLoaderT,
                   public InstanceSymbolsLoader2T,
                   public DeviceSymbolsLoaderT,
                   public DeviceSymbolsLoader2T
{
  private:
    std::unique_ptr<f6::bin::DynamicLibraryLoader> m_loader;

  public:
    ContextDLL() = delete;
    ContextDLL(const ContextDLL&) = delete;
    ContextDLL& operator=(const ContextDLL&) = delete;
    ContextDLL(ContextDLL&&) = delete;
    ContextDLL& operator=(ContextDLL&&) = delete;

    ContextDLL(const ContextCreateInfoT createInfo);

    void loadTop(const Instance* inst) override { InstanceSymbolsLoader2T::load(this, inst); }
    void loadBottom(const LogicalDevice* dev) override { DeviceSymbolsLoader2T::load(this, dev); }
};