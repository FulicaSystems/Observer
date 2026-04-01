#pragma once

#include <memory>
#include <unordered_map>
#include <utility>

#include <mutex>

#include "resource.hpp"

// TODO : maybe make this class an external library (new repository)
/**
 * @brief singleton object that loads and saves resources (cpu and gpu side)
 * TODO : this class is subject to rename (should it be called data manager ? since the data it
 * processes only concerns rendering data, meshes, images and else). as opposed to a collision
 * engine, that also might need a data manager, this class's data only concerns rendering objects.
 * it is possible to have a higher class DataManager with pImpls, one specifying rendering data et
 * the other collision data or other types of data, like videos and else.
 * TODO : multithreading, load gpu side only after cpu side is ready
 *
 */
class ResourceManager
{
  private:
    static std::unique_ptr<ResourceManager> m_instance;

  private:
    // TODO : is uint64_t really needed?
    uint64_t resourceCount = 0ULL;

    std::mutex resourcesMutex;
    std::unordered_map<ResourceLoadInfoT, std::shared_ptr<ResourceABC>> resources;

  public:
    template<class TResource>
        requires std::constructible_from<TResource, uint64_t, ResourceLoadInfoT*>
    static inline std::shared_ptr<TResource> load(const ResourceLoadInfoT loadInfo);

    static void clearAllResources();

  public:
    static ResourceManager& getInstance() { return *m_instance; }

    // TODO : rename
} typedef DataManager;
typedef ResourceManager RenderingDataManager;

template<class TResource>
    requires std::constructible_from<TResource, uint64_t, ResourceLoadInfoT*>
inline std::shared_ptr<TResource> ResourceManager::load(const ResourceLoadInfoT loadInfo)
{
    ResourceManager& rm = getInstance();

    // TODO : resource count
    auto resource = std::make_shared<TResource>(rm.resourceCount++, &loadInfo);
    // create local resource within the host resource constructor (pair)
    assert(resource->local);

    {
        std::lock_guard<std::mutex> guard(rm.resourcesMutex);
        rm.resources[loadInfo] = resource;
    }

    resource->load();
    resource->local->load();

    return std::dynamic_pointer_cast<TResource>(resource);
}