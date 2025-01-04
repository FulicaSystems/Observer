#pragma once

#include <vulkan/vulkan.h>

#include <vector>

class Context;
class LogicalDevice;
class Surface;

class SwapChain
{
  private:
    const Context &cx;
    const LogicalDevice &device;
    const Surface &surface;

  private:
    VkSwapchainKHR m_handle;

  public:
    VkFormat imageFormat;
    VkExtent2D imageExtent;

    std::vector<VkImage> images;
    std::vector<VkImageView> imageViews;

  public:
    SwapChain() = delete;
    SwapChain(const Context &cx, const LogicalDevice &device, const Surface &surface)
        : cx(cx), device(device), surface(surface)
    {
    }
    SwapChain(const SwapChain &) = delete;
    SwapChain &operator=(const SwapChain &) = delete;
    SwapChain(SwapChain &&) = delete;
    SwapChain &operator=(SwapChain &&) = delete;

    ~SwapChain();

    void readyUp();

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
