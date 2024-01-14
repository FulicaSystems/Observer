#pragma once

#include <memory>
#include <unordered_map>
#include <utility>

#include <mutex>

#include "utils/singleton.hpp"
#include "utils/multithread/globalthreadpool.hpp"

#include "device.hpp"

#include "resource.hpp"



class ResourceLoader : public Utils::Singleton<ResourceLoader>
{
	SINGLETON(ResourceLoader)

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
inline std::shared_ptr<TResource> ResourceLoader::load(TArg&&... ctorArgs)
{
	ResourceLoader& rm = getInstance();

	// TODO : resource count
	auto resource = std::make_shared<TResource>(rm.resourceCount, std::forward<TArg>(ctorArgs)...);

	{
		std::lock_guard<std::mutex> guard(rm.resourcesMutex);
		rm.resources[rm.resourceCount] = resource;
	}

	Utils::GlobalThreadPool::addTask([=]() {
		// TODO : multithreaded load (host then local)
		{
			resource->load();
		}
		{
			// create local resource within the host load function
			assert(resource->local);
			resource->local->load();
		}
		}, false); // TODO : multithread

	return std::dynamic_pointer_cast<TResource>(resource);
}