#pragma once

#include <vulkan/vulkan.h>

#include <vector>

#include "device/memory/image.hpp"

class ContextABC;
class LogicalDevice;
class Surface;

struct SwapChainCreateInfoT
{
    std::optional<const LogicalDevice*> device;
    const Surface* surface;

    VkSurfaceFormatKHR surfaceFormat;
    VkPresentModeKHR presentMode;
    VkExtent2D extent;

    ImageViewCreateInfoT viewCreateInfo;
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
    std::vector<std::shared_ptr<ImageView>> imageViews;

    // TODO
    std::shared_ptr<Image> depthImage;
    std::shared_ptr<ImageView> depthImageView;

    // TODO : atomic bool validate swapchain
    // TODO : recreate swapchain handle (swapchain recreation without affecting the pointers to it)

    SwapChain() = delete;
    SwapChain(const SwapChainCreateInfoT ci) : ci(ci) {}
    SwapChain(const SwapChain&) = delete;
    SwapChain& operator=(const SwapChain&) = delete;
    SwapChain(SwapChain&&) = delete;
    SwapChain& operator=(SwapChain&&) = delete;

    ~SwapChain();

  public:
    [[nodiscard]] inline VkSwapchainKHR& getHandle() { return m_handle; }
    [[nodiscard]] inline const VkSwapchainKHR& getHandle() const { return m_handle; }

    [[nodiscard]] inline VkSurfaceFormatKHR getSurfaceFormat() const { return ci.surfaceFormat; }

    [[nodiscard]] inline uint32_t getWidth() const { return imageExtent.width; }
    [[nodiscard]] inline uint32_t getHeight() const { return imageExtent.height; }

    [[nodiscard]] inline int getImageCount() const { return images.size(); }
    [[nodiscard]] inline const std::vector<std::shared_ptr<ImageView>>& getImageViews() const
    {
        return imageViews;
    }
    [[nodiscard]] inline const std::shared_ptr<ImageView>& getDepthImageView() const
    {
        return depthImageView;
    }
};
