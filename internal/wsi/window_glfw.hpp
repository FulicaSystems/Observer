#pragma once

#include <GLFW/glfw3.h>

#include "window.hpp"

class ContextABC;
class Instance;

class WSILoaderGLFW final : public WSILoaderI
{
  public:
    int init() override;
    void terminate() override;
};

class WindowGLFW final : public WindowI, public PresentWindowABC
{
  private:
    bool m_resizable = false;

    uint32_t m_width = 1366;
    uint32_t m_height = 768;

    int m_framebufferWidth;
    int m_framebufferHeight;

    GLFWwindow *m_handle;

  public:
    WindowGLFW() = delete;
    WindowGLFW(const WindowGLFW &) = delete;
    WindowGLFW &operator=(const WindowGLFW &) = delete;
    WindowGLFW(WindowGLFW &&) = delete;
    WindowGLFW &operator=(WindowGLFW &&) = delete;

    explicit WindowGLFW(const uint32_t width, const uint32_t height, const bool resizable = false);

    ~WindowGLFW();

    const std::vector<const char *> getRequiredExtensions() const override;

    void makeContextCurrent() override;
    Surface* createSurface(const ContextABC* cx, const Instance* inst) override;

    void swapBuffers() override;
    void pollEvents() override;

    bool shouldClose() const override;
};