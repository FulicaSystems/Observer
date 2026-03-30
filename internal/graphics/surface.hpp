#pragma once

#include <vulkan/vulkan.h>

#include <vector>

class Context;
class Instance;

// TODO : abstraction
struct SurfaceHandleT
{
    VkSurfaceKHR handle;
};

struct SurfaceCreateInfoT
{
    const Context *cx;
    const Instance *inst;
    // TODO : abstraction
    VkSurfaceKHR surface;
};

class Surface
{
  private:
    const SurfaceCreateInfoT ci;

  private:
    VkSurfaceKHR m_handle;

  public:
    Surface() = delete;
    /**
     * create a surface with an already existing handle
     */
    Surface(const SurfaceCreateInfoT ci) : ci(ci), m_handle(ci.surface)
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
