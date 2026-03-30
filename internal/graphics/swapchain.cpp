#include "context.hpp"
#include "device/device.hpp"
#include "surface.hpp"

#include "swapchain.hpp"

SwapChain::~SwapChain()
{
    ci.device->destroySwapChain(*this);
}
