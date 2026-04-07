#pragma once

#include <vector>

#include "mesh.hpp"

class RenderPass;
enum class BufferingTypeE;
class Buffer;

struct SceneLoadInfoT : public ResourceLoadInfoT
{
    std::optional<const RenderPass*> renderPass;
    BufferingTypeE type;
};

/**
 * @brief description of a scene made specifically for a rendering engine (this one, Observer)
 *
 */
class Scene : public ResourceABC
{
  private:
  public:
    void loadHost(const uint64_t index, const std::shared_ptr<ResourceLoadInfoT> loadInfo);
    void loadLocal(const std::shared_ptr<ResourceLoadInfoT> loadInfo);

    void unloadHost();
    void unloadLocal();
};

class CPUScene : public HostResourceABC
{
  public:
    // TODO : other objects that can be rendered such as billboards, or particles (later)
    std::vector<std::shared_ptr<Mesh>> m_meshes;

    CPUScene() = delete;
    CPUScene(uint64_t index) : HostResourceABC(index) {}
};

class GPUScene : public LocalResourceABC
{
  public:
    std::vector<std::unique_ptr<RenderState>> m_renderStates;
};
