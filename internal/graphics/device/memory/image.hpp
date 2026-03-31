#pragma once

#include <vulkan/vulkan.h>

struct ImageCreateInfoT
{
};

class Image
{
  public:
    VkImage handle;
};

struct ImageViewCreateInfoT
{
    std::optional<VkImage> image;
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