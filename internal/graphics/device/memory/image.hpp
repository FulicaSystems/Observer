#pragma once

#include <vulkan/vulkan.h>

#include <include/vk_mem_alloc.h>

struct ImageCreateInfoT
{
    VkImageType imageType = VK_IMAGE_TYPE_2D;
    VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;
    VkExtent3D extent;
    uint32_t mipLevels = 1U;
    uint32_t arrayLayers = 1U;
    VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;
    VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;
    VkImageUsageFlags usage = VK_IMAGE_USAGE_SAMPLED_BIT;
    VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    VkImageLayout initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
};

class Image
{
  public:
    /**
     * @brief allocation handle
     * TODO : handle abstraction
     *
     */
    VmaAllocation memory;
    VkImage handle;
};

struct ImageViewCreateInfoT
{
    /**
     * @brief if no image is specified, the creation function may retrieve itself
     *
     */
    std::optional<VkImage> image;
    /**
     * @brief same
     *
     */
    std::optional<VkFormat> format;
    VkImageAspectFlags aspect;
};

class ImageView
{
  public:
    VkImageView handle;
};

struct SamplerCreateInfoT
{
};

class Sampler
{
};