#pragma once

#include <vulkan/vulkan.hpp>

#include "graphics/device/asset/pipeline.hpp"
#include "graphics/device/device.hpp"

struct RenderStateCreateInfoT
{
    const LogicalDevice* deviceptr;
    PipelineCreateInfoT pipelineCreateInfo;

    virtual ~RenderStateCreateInfoT() = default;
};

class RenderStateABC
{
  public:
    VkDescriptorPool descriptorPool;
    std::unique_ptr<Pipeline> pipeline;

  public:
    RenderStateABC() = delete;
    explicit RenderStateABC(std::shared_ptr<RenderStateCreateInfoT> createInfo)
    {
        pipeline = createInfo->deviceptr->createPipeline(createInfo->pipelineCreateInfo);
    }

    virtual ~RenderStateABC() {}

} typedef RenderableABC, RenderObjectABC, RenderDescriptionABC;
