#include <cassert>

#include "context.hpp"
#include "device/device.hpp"
#include "surface.hpp"

#include "swapchain.hpp"

SwapChain::~SwapChain()
{
    assert(ci.device.has_value());
    ci.device.value()->destroySwapChain(*this);
}
