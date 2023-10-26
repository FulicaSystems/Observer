#pragma once

#include <memory>
#include <unordered_map>
#include <utility>

#include <mutex>

#include "utils/singleton.hpp"
#include "utils/multithread/globalthreadpool.hpp"

#include "device.hpp"

#include "resource.hpp"

class ResourcesManager : public Utils::Singleton<ResourcesManager>
{
	SINGLETON(ResourcesManager)

private:
	// TODO : is uint64_t really needed?
	uint64_t resourceCount = 0ULL;


	std::mutex resourcesMutex;
	std::unordered_map<uint64_t, std::shared_ptr<HostResourceABC>> resources;

public:
	template<class TResource, typename... TArg>
		requires std::constructible_from<TResource, uint64_t, TArg...>
	static inline std::shared_ptr<TResource> load(TArg&&... ctorArgs);

	static void clearAllResources();
};

template<class TResource, typename... TArg>
	requires std::constructible_from<TResource, uint64_t, TArg...>
inline std::shared_ptr<TResource> ResourcesManager::load(TArg&&... ctorArgs)
{
	ResourcesManager& rm = getInstance();

	// TODO : resource count
	auto resource = std::make_shared<TResource>(resourceCount, std::forward<TArg>(ctorArgs)...);

	{
		std::lock_guard<std::mutex> guard(rm.resourcesMutex);
		rm.resources[resourceCount] = resource;
	}

	Utils::GlobalThreadPool::addTask([=]() {
		// TODO : multithreaded load
		resource->load();
		resource->local->host = resource.get();
		resource->local->load();
		});

	return std::dynamic_pointer_cast<TResource>(resource);
}