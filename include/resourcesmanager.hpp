#pragma once

#include <memory>
#include <unordered_map>
#include <utility>

#include <mutex>

#include "utils/singleton.hpp"
#include "utils/multithread/globalthreadpool.hpp"

#include "resource.hpp"

class ResourcesManager : public Utils::Singleton<ResourcesManager>
{
	SINGLETON(ResourcesManager)

private:
	std::mutex resourcesMX;
	std::unordered_map<const char*, std::shared_ptr<IHostResource>> resources;

public:
	template<class TResource, typename... TArg>
		requires std::constructible_from<TResource, const char*&&, TArg...>
	static inline std::shared_ptr<IHostResource> load(const char*&& name, TArg&&... ctorArgs);
};

template<class TResource, typename... TArg>
	requires std::constructible_from<TResource, const char*&&, TArg...>
inline std::shared_ptr<IHostResource> ResourcesManager::load(const char*&& name, TArg&&... ctorArgs)
{
	ResourcesManager& rm = getInstance();

	std::shared_ptr<IHostResource> rsrc = std::make_shared<TResource>(std::move(name), std::forward<TArg>(ctorArgs)...);

	{
		std::lock_guard<std::mutex> guard(rm.resourcesMX);
		rm.resources[name] = rsrc;
	}

	Utils::GlobalThreadPool::addTask([&]() {
		rsrc->cpuLoad();
		rsrc->gpuLoad();
		});

	return rsrc;
}