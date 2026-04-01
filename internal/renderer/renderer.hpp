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

    /**
     * @brief acquire next image
     *
     */
    virtual void acquire() = 0;
    /**
     * @brief begin drawing/begin render pass
     *
     */
    virtual void begin() = 0;
    /**
     * @brief record draw commands relative to scene (or objects)
     *
     */
    virtual void draw(/*const Scene& scene*/) = 0;
    /**
     * @brief end drawing/end render pass
     *
     */
    virtual void end() = 0;
    /**
     * @brief submit command buffers
     *
     */
    virtual void submit() = 0;
    /**
     * @brief present last rendered frame
     *
     */
    virtual void present() = 0;
    /**
     * @brief swap back buffers
     *
     */
    virtual void swap() = 0;
};

// TODO : move to other file
class RenderState
{
  public:
    VkDescriptorPool descriptorPool;
    std::unique_ptr<Pipeline> pipeline;

} typedef Renderable, RenderObject;

/**
 * @brief the frame processor typedef states that the renderer primarily renders frame
 * TODO : soon to become a render phase within a render graph
 *
 */
class RendererABC
{
  protected:
    BufferingTypeE bufferingType = BufferingTypeE::DOUBLE_BUFFERING;

    std::vector<BackBufferT> backBuffers;

  public:
    virtual ~RendererABC() = default;

    virtual void render(/*const Scene& scene*/) = 0;

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

} typedef RenderPassBasedRenderer, LegacyRenderer;

class SinglePassRenderer : public RendererABC
{
  public:
    SinglePassRenderer() = delete;
    SinglePassRenderer(const RendererCreateInfoT* createInfo);

} typedef DynamicRenderer;
