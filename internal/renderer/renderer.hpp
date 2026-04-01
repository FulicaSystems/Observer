#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include "graphics/device/asset/pipeline.hpp"

class RendererI
{
  public:
    virtual ~RendererI() = 0;

    // virtual void renderFrame(Scene) = 0;
    virtual void presentFrame() = 0;
};

class RendererABC
{
  protected:
    BufferingTypeE bufferingType = BufferingTypeE::DOUBLE_BUFFERING;

  protected:
    VkDescriptorPool descriptorPool;
    std::unique_ptr<Pipeline> pipeline;

    std::vector<VkCommandBuffer> commandBuffers;

    // TODO : abstract semaphores for easier creation and destruction (and support multiple APIs)
    std::vector<VkSemaphore> drawSemaphores;
    std::vector<VkSemaphore> presentSemaphores;
    // TODO : abstract fences for same reasons
    std::vector<VkFence> inFlightFences;

    RendererABC(const LogicalDevice& device, const Swapchain& swapchain) : device(device);

  public:
    virtual ~RendererABC();
};

class MultiPassRenderer : public RendererABC
{
  private:
    std::vector<VkRenderPass> renderPasses;
    std::vector<VkFramebuffer> framebuffers;

  public:
    MultiPassRenderer() = delete;
    MultiPassRenderer(const LogicalDevice& device, const Swapchain& swapchain);
    ~MultiPassRenderer() override;
} typedef RenderPassBasedRenderer;
typedef MultiPassRenderer LegacyRenderer;

class SinglePassRenderer : public RendererABC
{
  public:
    SinglePassRenderer() = delete;
    SinglePassRenderer(const LogicalDevice& device, const Swapchain& swapchain);
    ~SinglePassRenderer() override;

} typedef DynamicRenderer;