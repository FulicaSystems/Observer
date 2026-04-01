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

struct RendererBackendCreateInfoT
{
  public:
    virtual ~RendererBackendCreateInfoT() {}
};

class RendererBackendABC : public RendererI
{
  protected:
    BufferingTypeE m_bufferingType = BufferingTypeE::DOUBLE_BUFFERING;
    std::vector<BackBufferT> m_backBuffers;

  public:
    RendererBackendABC() = delete;
    explicit RendererBackendABC(const std::shared_ptr<RendererBackendCreateInfoT> createInfo) {}

    virtual ~RendererBackendABC() override {}

} typedef RendererPImplABC;

struct LegacyRendererBackendCreateInfoT : RendererBackendCreateInfoT
{
    std::vector<std::shared_ptr<RenderPass>> renderPasses;
};

class MultiPassRendererBackend : public RendererBackendABC
{
  private:
    std::vector<std::shared_ptr<RenderPass>> m_renderPasses;
    std::vector<Framebuffer> m_framebuffers;

  public:
    MultiPassRendererBackend() = delete;
    MultiPassRendererBackend(const std::shared_ptr<RendererBackendCreateInfoT> createInfo);

    ~MultiPassRendererBackend() override {}

    void acquire() override;
    void begin() override;
    void draw(/*const Scene& scene*/) override;
    void end() override;
    void submit() override;
    void present() override;
    void swap() override;

} typedef RenderPassBasedRendererBackend, LegacyRendererBackend;

class SinglePassRendererBackend : public RendererBackendABC
{
  public:
    SinglePassRendererBackend() = delete;
    SinglePassRendererBackend(const std::shared_ptr<RendererBackendCreateInfoT> createInfo);

    ~SinglePassRendererBackend() override {}

    void acquire() override;
    void begin() override;
    void draw(/*const Scene& scene*/) override;
    void end() override;
    void submit() override;
    void present() override;
    void swap() override;

} typedef DynamicRendererBackend;

struct RendererCreateInfoT final
{
    const LogicalDevice* device;
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

} typedef FrameProcessor;
