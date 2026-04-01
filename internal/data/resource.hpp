#pragma once

#include <filesystem>
#include <memory>

class LogicalDevice;

struct ResourceLoadInfoT
{
    // device on which the resource will be allocated/created
    const LogicalDevice* deviceptr;

    // resource path if relevant
    const std::filesystem::path filepath;

    virtual ~ResourceLoadInfoT() = default;
    virtual std::size_t hash() const = 0;
};

class LoadableI
{
  public:
    virtual ~LoadableI() = 0;

    /**
     * @brief load host
     *
     */
    virtual void loadHost(const uint64_t index, const ResourceLoadInfoT* loadInfo) = 0;
    /**
     * @brief load local
     *
     */
    virtual void loadLocal(const ResourceLoadInfoT* loadInfo) = 0;

    /**
     * @brief unload host
     *
     */
    virtual void unloadHost() = 0;
    /**
     * @brief unload local
     *
     */
    virtual void unloadLocal() = 0;
};

class HostResourceABC;
class LocalResourceABC;

class ResourceABC : public LoadableI
{
  public:
    /**
     * @brief flag that tells if the resource is successfully loaded
     *
     */
    std::atomic_flag loaded = ATOMIC_FLAG_INIT;
    std::atomic_flag cpuSideLoaded = ATOMIC_FLAG_INIT;
    std::atomic_flag gpuSideLoaded = ATOMIC_FLAG_INIT;

    /**
     * @brief host part of this resource
     *
     */
    std::shared_ptr<HostResourceABC> hostResource;
    /**
     * @brief device (gpu) part of this resource
     *
     */
    std::shared_ptr<LocalResourceABC> localResource;

    virtual ~ResourceABC() = default;
};

// host accessible resource
// CPU host
class HostResourceABC
{
  public:
    const uint64_t m_index = 0ULL;
    std::filesystem::path m_filepath = "";

    HostResourceABC() = delete;
    /**
     * @brief Construct a new Host Resource A B C object
     * user must declare this contructor in the derived class
     *
     * @param index
     */
    explicit HostResourceABC(const uint64_t index) : m_index(index) {}

    inline uint64_t getIndex() const { return m_index; }

    // unload when destroying
    virtual ~HostResourceABC() = default;
};

/**
 * resources used for rendering with GPU
 * GPU device local
 */
class LocalResourceABC
{
  public:
    const LogicalDevice* deviceptr;

    virtual ~LocalResourceABC() = default;
};
