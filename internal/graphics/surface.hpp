#pragma once

#include <vulkan/vulkan.h>

#include <vector>

class Context;

class Surface
{
  private:
    const Context &cx;

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
    Surface(const Surface &) = delete;
    Surface &operator=(const Surface &) = delete;
    Surface(Surface &&) = delete;
    Surface &operator=(Surface &&) = delete;

    ~Surface();

  public:
    inline VkSurfaceKHR getHandle() const
    {
        return m_handle;
    }
};

struct SurfaceDetailsT
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;

    bool isPresentationReady() const;
    bool tryFindFormat(const VkFormat &targetFormat, const VkColorSpaceKHR &targetColorSpace,
                       VkSurfaceFormatKHR &found);
    bool tryFindPresentMode(const VkPresentModeKHR &targetPresentMode, VkPresentModeKHR &found);
    VkExtent2D findExtent(uint32_t width, uint32_t height);

} typedef SwapChainCompatibilityT;
