#pragma once

#include <memory>
#include <vector>

#include "engine/vertex.hpp"
#include "resource.hpp"

#include "graphics/device/memory/buffer.hpp"

class Mesh : public ResourceABC
{
  public:
    void loadHost(const uint64_t index, const ResourceLoadInfoT* loadInfo);
    void loadLocal(const ResourceLoadInfoT* loadInfo);

    void unloadHost();
    void unloadLocal();
};

class CPUMesh : public HostResourceABC
{
  public:
    std::vector<Vertex> vertices;

  public:
    CPUMesh() = delete;
    CPUMesh(uint64_t index) : HostResourceABC(index) {}

    inline const uint32_t getVertexCount() const { return vertices.size(); }
    inline const size_t getDataSize() const { return vertices.size() * sizeof(Vertex); }
    inline constexpr const std::vector<Vertex> getData() const { return vertices; }
    inline constexpr const Vertex* getRawData() const { return vertices.data(); }
};

class GPUMesh : public LocalResourceABC
{
  public:
    // CPU accessible data
    uint32_t vertexCount = 0;
    const void* vertices = nullptr;

    // buffer size
    size_t bufferSize = 0;

    // GPU data
    std::shared_ptr<Buffer> buffer;
};
