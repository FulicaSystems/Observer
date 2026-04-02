#include "graphics/device/device.hpp"

#include "mesh.hpp"

void Mesh::loadHost(const uint64_t index, const ResourceLoadInfoT* loadInfo)
{
    auto r = std::make_shared<CPUMesh>(index);
    hostResource = r;
    r->vertices.reserve(3);
    r->vertices.push_back(Vertex({
        {0.0f, -0.5f, 0.f},
        {0.0f, -0.5f, 0.f},
        {0.0f, -0.5f, 0.f, 1.f},
        {0.0f, -0.5f},
    }));
    r->vertices.push_back(Vertex({
        {0.5f, 0.5f, 0.f},
        {0.5f, 0.5f, 0.f},
        {0.5f, 0.5f, 0.f, 1.f},
        {0.5f, 0.5f},
    }));
    r->vertices.push_back(Vertex({
        {-0.5f, 0.5f, 0.f},
        {-0.5f, 0.5f, 0.f},
        {-0.5f, 0.5f, 0.f, 1.f},
        {-0.5f, 0.5f},
    }));

    cpuSideLoaded.test_and_set();
}
void Mesh::loadLocal(const ResourceLoadInfoT* loadInfo)
{
    auto r = std::make_shared<GPUMesh>();
    localResource = r;
    r->deviceptr = loadInfo->deviceptr;

    auto host = std::static_pointer_cast<CPUMesh>(hostResource);

    r->vertexCount = host->getVertexCount();
    r->vertices = host->getRawData();
    r->bufferSize = host->getDataSize();
    r->buffer = loadInfo->deviceptr->createBuffer(BufferCreateInfoT{
        .size = r->bufferSize,
        .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
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
    r->deviceptr->destroyBuffer(r->buffer);
}
