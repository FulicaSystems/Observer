#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include "device/asset/render_pass.hpp"
#include "device/memory/image.hpp"

struct FramebufferCreateInfoT
{
    const RenderPass* renderPass;
    std::vector<ImageView> attachments;
    uint32_t width;
    uint32_t height;
};

class Framebuffer
{
  public:
    VkFramebuffer handle;
};