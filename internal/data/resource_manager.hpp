#pragma once

#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>

#include <mutex>

#include "resource.hpp"

template<> class std::hash<ResourceLoadInfoT*>
{
  public:
    std::size_t operator()(const ResourceLoadInfoT* li) const { return li->hash(); }
};

// TODO : maybe make this class an external library (new repository), when moving this class in a
// new project, Mesh and Scene should remain in the data/saved folder because they are part of the
// internal structure of Observer
/**
 * @brief singleton object that loads and saves resources (cpu and gpu side)
 * TODO : this class is subject to rename (should it be called data manager ? since the data it
 * processes only concerns rendering data, meshes, images and else). as opposed to a collision
 * engine, that also might need a data manager, this class's data only concerns rendering
 * objects. it is possible to have a higher class DataManager with pImpls, one specifying
 * rendering data et the other collision data or other types of data, like videos and else.
 * TODO : multithreading, load gpu side only after cpu side is ready
 *
 */
class ResourceManager
{
  private:
    static std::unique_ptr<ResourceManager> m_instance;

  private:
    // TODO : is uint64_t really needed?
    std::atomic<uint64_t> resourceCount = 0ULL;

    std::mutex resourcesMutex;
    /**
     * @brief array of resources, pairing hash (size_t) and resource pointer (shared_ptr)
     *
     * TODO : find a way to make data of same type contiguous (for easier data oriented design or
     * optimization), therefore may not use unordered_map
     * TODO : make a data oriented resource manager (memory manager)
     *
     */
    std::unordered_map<std::size_t, std::shared_ptr<ResourceABC>> resources;

    static ResourceManager& getInstance() { return *m_instance; }

  public:
    template<class TResource>
        requires std::is_base_of<ResourceABC, TResource>::value
    static inline std::shared_ptr<TResource> load(const ResourceLoadInfoT* loadInfo);

    static void clearAllResources();

    // TODO : rename
} typedef DataManager, RenderingDataManager;

template<class TResource>
    requires std::is_base_of<ResourceABC, TResource>::value
inline std::shared_ptr<TResource> ResourceManager::load(const ResourceLoadInfoT* loadInfo)
{
    ResourceManager& rm = getInstance();

    // TODO : retrieve if resource already existing (or reload)

    ++resourceCount;
    auto resource = std::make_shared<TResource>();

    {
        std::lock_guard<std::mutex> guard(rm.resourcesMutex);
        // copy data from the load info structure, as it is an object rather than a pointer, the
        // derived load info data are lost but may not be required to retrieve the resource with the
        // loda info key
        std::size_t key = std::hash<ResourceLoadInfoT*>{}(loadInfo);
        rm.resources[key] = resource;
    }

    // TODO : multithreading
    resource->loadHost(resourceCount, loadInfo);
    resource->loadLocal(loadInfo);

    return std::dynamic_pointer_cast<TResource>(resource);
}