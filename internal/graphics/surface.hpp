#pragma once

#include <vulkan/vulkan.h>

class Context;

class Surface
{
  private:
    const Context& cx;

  private:
    VkSurfaceKHR m_handle;

  public:
    Surface() = delete;
    /**
     * create a surface with an already existing handle
     */
    Surface(const Context &cx, const VkSurfaceKHR &surface) : cx(cx), m_handle(surface)
    {
    }
    Surface(const Surface&) = delete;
    Surface& operator=(const Surface&) = delete;
    Surface(Surface&&) = delete;
    Surface& operator=(Surface&&) = delete;

    ~Surface();

    public:
    inline VkSurfaceKHR getHandle() const { return m_handle; }
};
