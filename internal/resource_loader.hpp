#pragma once

#include <memory>
#include <unordered_map>
#include <utility>

#include <mutex>

#include "utils/multithread/globalthreadpool.hpp"
#include "utils/singleton.hpp"

#include "device.hpp"

#include "resource.hpp"

class ResourceLoader : public Utils::Singleton<ResourceLoader>
{
    SINGLETON(ResourceLoader)

  private:
    // TODO : is uint64_t really needed?
    uint64_t resourceCount = 0ULL;

    std::mutex resourcesMutex;
    std::unordered_map<ResourceLoadInfoI, std::shared_ptr<HostResourceABC>> resources;

  public:
    template <class TResource>
        requires std::constructible_from<TResource, uint64_t, ResourceLoadInfoI *>
    static inline std::shared_ptr<TResource> load(const ResourceLoadInfoI &loadInfo);

    static void clearAllResources();
};

template <class TResource>
    requires std::constructible_from<TResource, uint64_t, ResourceLoadInfoI *>
inline std::shared_ptr<TResource> ResourceLoader::load(const ResourceLoadInfoI &loadInfo)
{
    ResourceLoader &rm = getInstance();

    // TODO : resource count
    auto resource = std::make_shared<TResource>(rm.resourceCount++, &loadInfo);
    // create local resource within the host resource constructor (pair)
    assert(resource->local);

    {
        std::lock_guard<std::mutex> guard(rm.resourcesMutex);
        rm.resources[loadInfo] = resource;
    }

    Utils::GlobalThreadPool::addTask(
        [=]() {
            // TODO : multithreaded load (host then local)
            {
                resource->load();
            }
            {
                resource->local->load();
            }
        },
        false); // TODO : multithread

    return std::dynamic_pointer_cast<TResource>(resource);
}