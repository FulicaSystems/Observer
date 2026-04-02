#pragma once

#include <vulkan/vulkan.hpp>

#include "data/saved/mesh.hpp"
#include "graphics/device/asset/pipeline.hpp"

class RenderStateABC
{
  public:
    VkDescriptorPool descriptorPool;
    std::unique_ptr<Pipeline> pipeline;

    virtual RenderStateABC() {}

} typedef RenderableABC, RenderObjectABC, RenderDescriptionABC;

class MeshRenderDescription : public RenderStateABC
{
  private:
    // TODO : uniform block/uniform buffer
    std::shared_ptr<GPUMesh> m_meshes;
};