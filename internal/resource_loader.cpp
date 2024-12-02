#include <iostream>

#include "resourceloader.hpp"

void ResourceLoader::clearAllResources()
{
    ResourceLoader &rm = getInstance();

    std::lock_guard<std::mutex> guard(rm.resourcesMutex);
    for (auto &r : rm.resources)
    {
        // TODO : clear (unload) one by one
        std::cout << "Clearing " << r.second->getIndex() << " : " << r.second.use_count() << " use" << std::endl;
    }
    rm.resources.clear();
}