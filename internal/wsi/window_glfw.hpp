#pragma once

#include <GLFW/glfw3.h>

#include "window.hpp"

class WSILoaderGLFW final : public WSILoaderI
{
  public:
    int init() override;
    void terminate() override;
};

class WindowGLFW final : public WindowI
{
  private:
    // std::unique_ptr<Swapchain> m_swapchain;
    // std::unique_ptr<Surface> m_surface;

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

    WindowGLFW(const uint32_t width, const uint32_t height, const bool resizable = false);

    ~WindowGLFW();

    const std::vector<const char *> getRequiredExtensions() const override;

    void makeContextCurrent() override;
    void swapBuffers() override;
    void pollEvents() override;
    bool shouldClose() const override;

    // const Surface& createSurface(const Instance& instance);
};