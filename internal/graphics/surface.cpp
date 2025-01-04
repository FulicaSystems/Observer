#include "context.hpp"

#define STD_INT
#define STD_LIMITS
#include "mathematics/mathematics.hpp"

#include "surface.hpp"

Surface::~Surface()
{
    cx.vkDestroySurfaceKHR(cx.getInstanceHandle(), m_handle, nullptr);
}

bool SurfaceDetailsT::tryFindFormat(const VkFormat &targetFormat, const VkColorSpaceKHR &targetColorSpace,
                                    VkSurfaceFormatKHR &found)
{
    for (const auto &format : formats)
    {
        if (format.format == targetFormat && format.colorSpace == targetColorSpace)
        {
            found = format;
            return true;
        }
    }

    return false;
}

bool SurfaceDetailsT::tryFindPresentMode(const VkPresentModeKHR &targetPresentMode, VkPresentModeKHR &found)
{
    for (const auto &presentMode : presentModes)
    {
        if (presentMode == targetPresentMode)
        {
            found = presentMode;
            return true;
        }
    }

    return false;
}

VkExtent2D SurfaceDetailsT::findExtent(uint32_t width, uint32_t height)
{
    if (capabilities.currentExtent.width != (std::numeric_limits<uint32_t>::max)())
    {
        return capabilities.currentExtent;
    }
    else
    {
        return VkExtent2D{
            .width = Math::clamp(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
            .height = Math::clamp(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height),
        };
    }
}
