#pragma once

#include <memory>
#include <unordered_map>
#include <utility>

#include "resource.hpp"

class ResourcesManager
{
private:
    std::unordered_map<const char*, std::shared_ptr<IHostResource>> resources;

public:
    template<class TResource, typename... TArg>
        requires std::constructible_from<TResource, const char*&&, TArg...>
    inline std::shared_ptr<IHostResource> load(const char*&& name, TArg&&... ctorArgs);
};

template<class TResource, typename... TArg>
    requires std::constructible_from<TResource, const char*&&, TArg...>
inline std::shared_ptr<IHostResource> ResourcesManager::load(const char*&& name, TArg&&... ctorArgs)
{
    std::shared_ptr<IHostResource> rsrc = std::make_shared<TResource>(std::move(name), std::forward<TArg>(ctorArgs)...);

    resources[name] = rsrc;

    rsrc->cpuLoad();
    rsrc->gpuLoad();

    return rsrc;
}