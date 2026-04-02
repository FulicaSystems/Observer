#include <functional>

#include "graphics/device/device.hpp"

#include "mesh.hpp"

void Mesh::loadHost(const uint64_t index, const std::shared_ptr<ResourceLoadInfoT> loadInfo)
{
    auto r = std::make_shared<CPUMesh>(index);
    hostResource = r;
    // TODO : assimp
    r->vertices = {
        {{-0.5f, -0.5f, 0.f},   {-0.5f, -0.5f, 0.f},   {1.f, 0.f, 0.f, 1.f}, {1.f, 0.f}},
        {{0.5f, -0.5f, 0.f},    {0.5f, -0.5f, 0.f},    {0.f, 1.f, 0.f, 1.f}, {0.f, 0.f}},
        {{0.5f, 0.5f, 0.f},     {0.5f, 0.5f, 0.f},     {0.f, 0.f, 1.f, 1.f}, {0.f, 1.f}},
        {{-0.5f, 0.5f, 0.f},    {-0.5f, 0.5f, 0.f},    {1.f, 1.f, 1.f, 1.f}, {1.f, 1.f}},
        {{-0.5f, -0.5f, -0.5f}, {-0.5f, -0.5f, -0.5f}, {1.f, 0.f, 0.f, 1.f}, {1.f, 0.f}},
        {{0.5f, -0.5f, -0.5f},  {0.5f, -0.5f, -0.5f},  {0.f, 1.f, 0.f, 1.f}, {0.f, 0.f}},
        {{0.5f, 0.5f, -0.5f},   {0.5f, 0.5f, -0.5f},   {0.f, 0.f, 1.f, 1.f}, {0.f, 1.f}},
        {{-0.5f, 0.5f, -0.5f},  {-0.5f, 0.5f, -0.5f},  {1.f, 1.f, 1.f, 1.f}, {1.f, 1.f}}
    };

    r->indices = {0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4};

    cpuSideLoaded.test_and_set();
}
void Mesh::loadLocal(const std::shared_ptr<ResourceLoadInfoT> loadInfo)
{
    auto r = std::make_shared<GPUMesh>();
    localResource = r;
    r->deviceptr = loadInfo->deviceptr;

    auto host = std::static_pointer_cast<CPUMesh>(hostResource);

    r->vertexCount = host->getVertexCount();
    r->vertices = host->getRawData();
    r->bufferSize = host->getVertexDataSize();
    r->vertexBuffer = loadInfo->deviceptr->createBuffer(BufferCreateInfoT{
        .size = r->bufferSize,
        .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    });
    r->indexCount = host->indices.size();
    r->indexBuffer = loadInfo->deviceptr->createBuffer(BufferCreateInfoT{
        .size = host->getIndexDataSize(),
        .usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    });

    gpuSideLoaded.test_and_set();
    loaded.test_and_set();
}

void Mesh::unloadHost()
{
    std::static_pointer_cast<CPUMesh>(hostResource)->vertices.clear();
}
void Mesh::unloadLocal()
{
    auto r = std::static_pointer_cast<GPUMesh>(localResource);
    r->deviceptr->destroyBuffer(r->vertexBuffer);
    r->deviceptr->destroyBuffer(r->indexBuffer);
}

std::size_t MeshLoadInfoT::hash() const
{
    std::size_t h = 0;
    if (vertices.has_value())
    {
        for (int i = 0; i < vertices.value().size(); ++i)
        {
            h += std::hash<float>{}(vertices.value()[i].position.x);
        }
        return h;
    }
    else
    {
        return ResourceLoadInfoT::hash();
    }
}
