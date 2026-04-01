#pragma once

#include <memory>
#include <vector>

#include <vulkan/vulkan.h>

#include "graphics/backbuffer.hpp"
#include "graphics/device/asset/render_pass.hpp"
#include "graphics/device/device.hpp"
#include "graphics/framebuffer.hpp"
#include "graphics/swapchain.hpp"

class RendererI
{
  public:
    virtual ~RendererI() {};

    /**
     * @brief begin drawing/begin render pass
     *
     */
    virtual void begin() = 0;
    /**
     * @brief record draw commands relative to scene (or objects)
     *
     */
    // TODO
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
    // TODO
    virtual void submit(/*std::vector<VkSemaphore> additionalWaitSemaphores, std::vector<VkSemaphore> additionalSignalSemaphores*/) = 0;
    /**
     * @brief swap back buffers
     *
     */
    virtual void swap() = 0;
};

class SwapChainRendererI
{
  public:
    /**
     * @brief acquire next swap chain image
     *
     */
    // TODO
    virtual uint32_t acquire(/*const SwapChain* swapchain*/) = 0;
    /**
     * @brief present last rendered frame
     *
     */
    // TODO
    virtual void present(/*const SwapChain* swapchain*/) = 0;
};

struct RendererBackendCreateInfoT
{
    BufferingTypeE bufferingType = BufferingTypeE::DOUBLE_BUFFERING;
    const LogicalDevice* device;
    uint32_t renderingWidth;
    uint32_t renderingHeight;

  public:
    virtual ~RendererBackendCreateInfoT() {}
};

class RendererBackendABC : public RendererI
{
  protected:
    BufferingTypeE m_bufferingType = BufferingTypeE::DOUBLE_BUFFERING;
    std::vector<std::shared_ptr<BackBufferT>> m_backBuffers;
    uint32_t m_currentBackBufferIndex = 0;

    const LogicalDevice* m_device;

  public:
    RendererBackendABC() = delete;
    explicit RendererBackendABC(const std::shared_ptr<RendererBackendCreateInfoT> createInfo);

    virtual ~RendererBackendABC() override {}

    virtual void swap() override;

} typedef RendererPImplABC;

struct LegacyRendererBackendCreateInfoT : RendererBackendCreateInfoT
{
    std::vector<std::shared_ptr<RenderPass>> renderPasses;
    std::vector<ImageView> colorAttachmentImageViews;
    std::optional<ImageView> depthAttachmentImageView;
};

class MultiPassRendererBackend : public RendererBackendABC, public SwapChainRendererI
{
  private:
    uint32_t m_currentRenderPassIndex = 0;
    std::vector<std::shared_ptr<RenderPass>> m_renderPasses;
    /**
     * @brief one group of framebuffers per render pass
     *
     */
    std::vector<std::vector<std::shared_ptr<Framebuffer>>> m_framebuffers;

  public:
    MultiPassRendererBackend() = delete;
    MultiPassRendererBackend(const std::shared_ptr<RendererBackendCreateInfoT> createInfo);

    ~MultiPassRendererBackend() override {}

    uint32_t acquire() override;

    void begin() override;
    void draw(/*const Scene& scene*/) override;
    void end() override;
    void submit() override;

    void present() override;

} typedef RenderPassBasedRendererBackend, LegacyRendererBackend;

class SinglePassRendererBackend : public RendererBackendABC, public SwapChainRendererI
{
  private:
  public:
    SinglePassRendererBackend() = delete;
    SinglePassRendererBackend(const std::shared_ptr<RendererBackendCreateInfoT> createInfo);

    ~SinglePassRendererBackend() override {}

    uint32_t acquire() override;

    void begin() override;
    void draw(/*const Scene& scene*/) override;
    void end() override;
    void submit() override;

    void present() override;

} typedef DynamicRendererBackend;

struct RendererCreateInfoT final
{
    const LogicalDevice* device;
    // TODO : pass image views instead of swapchain (renderer would then be able to render in
    // independent images)
    const SwapChain* swapchain;
    std::unique_ptr<RendererBackendABC> backend;
};

/**
 * @brief the frame processor typedef states that the renderer primarily renders frame
 * TODO : soon to become a render phase within a render graph
 *
 */
class Renderer
{
  private:
    std::unique_ptr<RendererBackendABC> m_backend;

  public:
    Renderer() = delete;
    Renderer(RendererCreateInfoT createInfo);

    void render(/*const Scene& scene*/);
    void swap();

} typedef FrameProcessor, RendererFrontend;
