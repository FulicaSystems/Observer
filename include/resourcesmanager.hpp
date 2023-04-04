#pragma once

#include <memory>
#include <unordered_map>
#include <utility>

#include "resource.hpp"

class ResourcesManager
{
private:
    std::unordered_map<const char*, std::shared_ptr<IResource>> resources;

public:
    template<class TR, typename... TArg>
        requires std::constructible_from<TR, const char*&&, TArg...>
    inline std::shared_ptr<IResource> load(const char*&& name, TArg&&... ctorArgs);
};

template<class TR, typename... TArg>
    requires std::constructible_from<TR, const char*&&, TArg...>
inline std::shared_ptr<IResource> ResourcesManager::load(const char*&& name, TArg&&... ctorArgs)
{
    std::shared_ptr<IResource> rsrc = std::make_shared<TR>(std::move(name), std::forward<TArg>(ctorArgs)...);

    resources[name] = rsrc;

    rsrc->cpuLoad();
    rsrc->gpuLoad();

    return rsrc;
}