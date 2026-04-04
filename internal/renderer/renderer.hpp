#pragma once

#include <memory>
#include <vector>

#include <vulkan/vulkan.h>

#include "graphics/backbuffer.hpp"
#include "graphics/device/asset/render_pass.hpp"
#include "graphics/device/device.hpp"
#include "graphics/framebuffer.hpp"
#include "graphics/swapchain.hpp"

class Scene;

class RendererI
{
  public:
    virtual ~RendererI() {};

    virtual void wait() const = 0;
    /**
     * @brief begin drawing/begin render pass
     *
     */
    virtual void begin(const Framebuffer* framebuffer) const = 0;
    /**
     * @brief record draw commands relative to scene (or objects)
     * draw indexed
     * TODO : make a draw not indexed version ?
     *
     */
    virtual void draw(const std::shared_ptr<Scene> scene) const = 0;
    /**
     * @brief end drawing/end render pass
     *
     */
    virtual void end() const = 0;
    /**
     * @brief submit command buffers
     *
     */
    virtual void submit() const = 0;
    /**
     * @brief swap back buffers (only affect the back buffer index)
     *
     */
    virtual void swap() = 0;
};

class SwapChainRendererI
{
  public:
    typedef std::pair<const SwapChain*, uint32_t> AvailableImage;

    virtual void addSwapChain(const SwapChain* swapchain) = 0;

    /**
     * @brief acquire next swap chain image
     *
     */
    virtual std::vector<uint32_t> acquire() = 0;
    /**
     * @brief present last rendered frame
     *
     */
    virtual void present() const = 0;
};

struct RendererBackendCreateInfoT
{
    BufferingTypeE bufferingType = BufferingTypeE::DOUBLE_BUFFERING;
    const LogicalDevice* device;

  public:
    virtual ~RendererBackendCreateInfoT() {}
};

/**
 * @brief backend for the renderer class
 * TODO : soon to become a render phase within a render graph
 *
 */
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

    void wait() const override;
    void swap() override;

} typedef RendererPImplABC;

struct LegacyRendererBackendCreateInfoT : RendererBackendCreateInfoT
{
    RenderPassCreateInfoT renderPassCreateInfo;
};

class LegacyRendererBackend : public RendererBackendABC, public SwapChainRendererI
{
  private:
    std::unique_ptr<RenderPass> m_renderPass;
    std::vector<const SwapChain*> m_swapchains;
    std::vector<uint32_t> m_currentSwapchainImageIndices;
    /**
     * @brief one group of framebuffers per render pass
     *
     */
    std::vector<std::vector<std::shared_ptr<Framebuffer>>> m_framebuffers;

  public:
    LegacyRendererBackend() = delete;
    LegacyRendererBackend(const std::shared_ptr<RendererBackendCreateInfoT> createInfo);

    ~LegacyRendererBackend() override {}

    void addSwapChain(const SwapChain* swapchain) override { m_swapchains.emplace_back(swapchain); }

    std::vector<uint32_t> acquire() override;

    void begin(const Framebuffer* framebuffer) const override;
    void draw(const std::shared_ptr<Scene> scene) const override;
    void end() const override;
    void submit() const override;

    void present() const override;

  public:
    [[nodiscard]] const RenderPass* getRenderPass() const { return m_renderPass.get(); }

} typedef RenderPassBasedRendererBackend;

class DynamicRendererBackend : public RendererBackendABC, public SwapChainRendererI
{
  private:
    std::vector<const SwapChain*> m_swapchains;

  public:
    DynamicRendererBackend() = delete;
    DynamicRendererBackend(const std::shared_ptr<RendererBackendCreateInfoT> createInfo);

    ~DynamicRendererBackend() override {}

    void addSwapChain(const SwapChain* swapchain) override { m_swapchains.emplace_back(swapchain); }

    std::vector<uint32_t> acquire() override;

    void begin(const Framebuffer* framebuffer) const override;
    void draw(const std::shared_ptr<Scene> scene) const override;
    void end() const override;
    void submit() const override;

    void present() const override;

} typedef StateBasedRendererBackend;

struct RendererCreateInfoT final
{
    const LogicalDevice* device;
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

    void render(const Framebuffer* framebuffer, const std::shared_ptr<Scene> scene);

  public:
    [[nodiscard]] inline const RendererBackendABC* getBackend() const { return m_backend.get(); }
    [[nodiscard]] inline RendererBackendABC* getBackend() { return m_backend.get(); }

} typedef FrameProcessor, RendererFrontend;
