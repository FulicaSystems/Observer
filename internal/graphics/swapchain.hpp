#pragma once

#include <vulkan/vulkan.h>

#include <vector>

class Context;
class LogicalDevice;
class Surface;

struct SwapChainCreateInfoT
{
    const Context *cx;
    const LogicalDevice *device;
    const Surface *surface;
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
