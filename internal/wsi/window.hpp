#pragma once

#include <vector>
#include <memory>

class Context;
class Instance;
class SwapChain;
class Surface;

class WSILoaderI
{
  public:
    virtual int init() = 0;
    virtual void terminate() = 0;
};

class WindowI
{
  public:
    virtual ~WindowI() = default;

    virtual const std::vector<const char *> getRequiredExtensions() const = 0;

    virtual void makeContextCurrent() = 0;
    virtual Surface *createSurface(const Context *cx, const Instance *inst) = 0;

    virtual void swapBuffers() = 0;
    virtual void pollEvents() = 0;

    virtual bool shouldClose() const = 0;
};

class PresentWindowABC
{
  protected:
    std::unique_ptr<SwapChain> m_swapchain;
    std::unique_ptr<Surface> m_surface;

  public:
    virtual ~PresentWindowABC()
    {
        m_swapchain.reset();
        m_surface.reset();
    }

    [[nodiscard]] const SwapChain *getSwapChain() const
    {
        return m_swapchain.get();
    }
    [[nodiscard]] const Surface *getSurface() const
    {
        return m_surface.get();
    }

    void setSwapChain(std::unique_ptr<SwapChain> swapchain)
    {
        m_swapchain = std::move(swapchain);
    }
};