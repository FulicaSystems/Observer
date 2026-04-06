#pragma once

#include <vector>

#include <vulkan/vulkan.hpp>

#include "graphics/device/asset/pipeline.hpp"
#include "graphics/device/device.hpp"

struct RenderStateCreateInfoT final
{
    const LogicalDevice* deviceptr;
    PipelineCreateInfoT pipelineCreateInfo;
};

class RenderableABC
{
  public:
    virtual ~RenderableABC() {}

} typedef RenderObjectABC, RenderDescriptionABC;

class RenderState final
{
  private:
    std::unique_ptr<Pipeline> pipeline;

    std::vector<std::shared_ptr<RenderableABC>> m_objects;

  public:
    RenderState() = delete;
    explicit RenderState(const RenderStateCreateInfoT createInfo)
    {
        pipeline = createInfo.deviceptr->createPipeline(createInfo.pipelineCreateInfo);
    }

    void addObject(std::shared_ptr<RenderableABC> object) { m_objects.push_back(object); }

  public:
    [[nodiscard]] const Pipeline* getPipeline() const { return pipeline.get(); }
    [[nodiscard]] const std::vector<std::shared_ptr<RenderableABC>>& getObjects() const
    {
        return m_objects;
    }
} typedef PipelineState;
