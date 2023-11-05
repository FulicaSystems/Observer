#include <iostream>

#include "resourcesmanager.hpp"



void ResourcesManager::clearAllResources()
{
	ResourcesManager& rm = getInstance();

	std::lock_guard<std::mutex> guard(rm.resourcesMutex);
	for (auto& r : rm.resources)
	{
		std::cout << "Clearing " << r.first << " : " << r.second.use_count() << " use" << std::endl;
	}
	rm.resources.clear();
}