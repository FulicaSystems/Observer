#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include "graphics/backbuffer.hpp"
#include "graphics/device/asset/pipeline.hpp"

class LogicalDevice;
class SwapChain;

struct RendererCreateInfoT
{
    const LogicalDevice* device;
    const SwapChain* swapchain;
};

class RendererI
{
  public:
    virtual ~RendererI() = 0;

    // virtual void renderFrame(Scene) = 0;
    virtual void presentFrame() = 0;
};

/**
 * @brief the frame processor typedef states that the renderer primarily renders frame
 * TODO : soon to become a render phase within a render graph
 *
 */
class RendererABC
{
  protected:
    BufferingTypeE bufferingType = BufferingTypeE::DOUBLE_BUFFERING;

  protected:
    VkDescriptorPool descriptorPool;
    std::unique_ptr<Pipeline> pipeline;

    std::vector<BackBufferT> backBuffers;

  public:
    virtual ~RendererABC() = default;

} typedef FrameProcessorABC;

struct LegacyRendererCreateInfoT
{
    std::vector<RenderPass> renderPasses;
};

class MultiPassRenderer : public RendererABC
{
  private:
    // TODO : move array of render pass from create info to this variable
    std::vector<RenderPass> renderPasses;
    std::vector<VkFramebuffer> framebuffers;

  public:
    MultiPassRenderer() = delete;
    MultiPassRenderer(const RendererCreateInfoT* createInfo);

} typedef RenderPassBasedRenderer;
typedef MultiPassRenderer LegacyRenderer;

class SinglePassRenderer : public RendererABC
{
  public:
    SinglePassRenderer() = delete;
    SinglePassRenderer(const RendererCreateInfoT* createInfo);

} typedef DynamicRenderer;
