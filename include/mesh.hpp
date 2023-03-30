#pragma once

#include <vector>

#include "vertex.hpp"
#include "resource.hpp"

struct GPUMesh : public IGPUResource
{
};

class Mesh : public IResource
{
private:
    std::vector<Vertex> vertices;
};