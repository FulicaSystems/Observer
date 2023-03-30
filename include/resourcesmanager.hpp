#pragma once

#include <memory>
#include <unordered_map>

#include "resource.hpp"

class ResourcesManager
{
private:
    std::unordered_map<std::shared_ptr<IResource>> resources;

public:
};