#include "resourcesmanager.hpp"

void ResourcesManager::clearAllResources()
{
	ResourcesManager& rm = getInstance();

	std::lock_guard<std::mutex> guard(rm.resourcesMX);
	for (auto& r : rm.resources)
	{
		printf("%i", r.second.use_count());
	}
	rm.resources.clear();
}