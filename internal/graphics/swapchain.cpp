#include "context.hpp"
#include "device/device.hpp"
#include "surface.hpp"

#include "swapchain.hpp"

SwapChain::~SwapChain()
{
    for (VkImageView &imageView : imageViews)
    {
        device.vkDestroyImageView(device.getHandle(), imageView, nullptr);
    }
    device.vkDestroySwapchainKHR(device.getHandle(), m_handle, nullptr);
}

void SwapChain::readyUp()
{
    
}
