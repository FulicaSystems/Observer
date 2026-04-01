#pragma once

#include <vulkan/vulkan.hpp>

#include "graphics/device/asset/pipeline.hpp"

class RenderState
{
  public:
    VkDescriptorPool descriptorPool;
    std::unique_ptr<Pipeline> pipeline;

} typedef Renderable, RenderObject;
