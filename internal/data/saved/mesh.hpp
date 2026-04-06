#pragma once

#include <memory>
#include <optional>
#include <vector>

#include "engine/vertex.hpp"
#include "resource.hpp"

#include "graphics/device/memory/buffer.hpp"

#include "renderer/render_state.hpp"

struct MeshLoadInfoT : public ResourceLoadInfoT
{
    std::optional<std::vector<Vertex>> vertices;
    std::optional<std::vector<uint16_t>> indices;

    virtual std::size_t hash() const override;
};

class Mesh : public ResourceABC
{
  public:
    void loadHost(const uint64_t index, const std::shared_ptr<ResourceLoadInfoT> loadInfo);
    void loadLocal(const std::shared_ptr<ResourceLoadInfoT> loadInfo);

    void unloadHost();
    void unloadLocal();
};

class CPUMesh : public HostResourceABC
{
  public:
    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;

  public:
    CPUMesh() = delete;
    CPUMesh(uint64_t index) : HostResourceABC(index) {}

    inline const uint32_t getVertexCount() const { return vertices.size(); }
    inline const size_t getVertexDataSize() const { return vertices.size() * sizeof(Vertex); }
    inline const size_t getIndexDataSize() const { return indices.size() * sizeof(uint16_t); }
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
    int indexCount = -1;

    // GPU data
    std::shared_ptr<Buffer> vertexBuffer;
    std::shared_ptr<Buffer> indexBuffer;
};

class MeshRenderDescription : public RenderableABC
{
  private:
    std::vector<std::unique_ptr<Buffer>> m_uniformBuffer;
    std::vector<std::vector<void*>> m_hostUniformBufferData;

    /**
     * @brief description of the mesh GPU side
     *
     */
    std::shared_ptr<GPUMesh> meshView;

  public:
    MeshRenderDescription() = delete;
    MeshRenderDescription(std::shared_ptr<GPUMesh> meshView) : meshView(meshView) {}

    const GPUMesh* getGPUMesh() const { return meshView.get(); }
};
