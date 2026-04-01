#pragma once

#include <vulkan/vulkan.hpp>

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
    std::unique_ptr<Mesh> m_meshes;
};