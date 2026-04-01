#include <iostream>

#include "resource_manager.hpp"

std::unique_ptr<ResourceManager> ResourceManager::m_instance = std::make_unique<ResourceManager>();

void ResourceManager::clearAllResources()
{
    ResourceManager& rm = getInstance();

    std::lock_guard<std::mutex> guard(rm.resourcesMutex);
    for (auto& r : rm.resources)
    {
        std::cout << "Clearing " << r.second->hostResource->getIndex() << " : "
                  << r.second.use_count() << " use" << std::endl;

        r.second->unloadHost();
        r.second->unloadLocal();
    }
    rm.resources.clear();
}