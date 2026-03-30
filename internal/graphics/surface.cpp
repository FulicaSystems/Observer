#include "context.hpp"

#define STD_INT
#define STD_LIMITS
#include "fumathematics/mathematics.hpp"

#include "surface.hpp"

Surface::~Surface()
{
    ci.cx->vkDestroySurfaceKHR(ci.inst->getHandle(), m_handle, nullptr);
}

std::optional<VkSurfaceFormatKHR> SurfaceDetailsT::findFormat(const VkFormat &targetFormat,
                                                              const VkColorSpaceKHR &targetColorSpace, int &index)
{
    int i = 0;
    for (const auto &format : formats)
    {
        if (format.format == targetFormat && format.colorSpace == targetColorSpace)

        {
            index = i;
            return std::make_optional<VkSurfaceFormatKHR>(format);
        }
        ++i;
    }
    i = -1;
    return std::nullopt;
}

std::optional<VkPresentModeKHR> SurfaceDetailsT::findPresentMode(const VkPresentModeKHR &targetPresentMode, int &index)
{
    int i = 0;
    for (const auto &presentMode : presentModes)
    {
        if (presentMode == targetPresentMode)

        {
            index = i;
            return std::make_optional<VkPresentModeKHR>(presentMode);
        }
        ++i;
    }
    i = -1;
    return std::nullopt;
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
