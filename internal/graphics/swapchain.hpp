#pragma once

#include <vulkan/vulkan.h>

#include <vector>

class ContextABC;
class LogicalDevice;
class Surface;

struct SwapChainCreateInfoT
{
    std::optional<const LogicalDevice *> device;
    const Surface *surface;

    VkSurfaceFormatKHR surfaceFormat;
    VkPresentModeKHR presentMode;
    VkExtent2D extent;
};

class SwapChain
{
  private:
    const SwapChainCreateInfoT ci;

  private:
    VkSwapchainKHR m_handle;

  public:
    VkFormat imageFormat;
    VkExtent2D imageExtent;

    std::vector<VkImage> images;
    std::vector<VkImageView> imageViews;

  public:
    SwapChain() = delete;
    SwapChain(const SwapChainCreateInfoT ci) : ci(ci)
    {
    }
    SwapChain(const SwapChain &) = delete;
    SwapChain &operator=(const SwapChain &) = delete;
    SwapChain(SwapChain &&) = delete;
    SwapChain &operator=(SwapChain &&) = delete;

    ~SwapChain();

  public:
    [[nodiscard]] inline VkSwapchainKHR &getHandle()
    {
        return m_handle;
    }
    [[nodiscard]] inline const VkSwapchainKHR &getHandle() const
    {
        return m_handle;
    }
};
