#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include "device/asset/render_pass.hpp"
#include "device/memory/image.hpp"

struct FramebufferCreateInfoT
{
    const RenderPass* renderPass;
    std::vector<std::shared_ptr<ImageView>> attachments;
    uint32_t width;
    uint32_t height;
};

class Framebuffer
{
  public:
    uint32_t width;
    uint32_t height;

    VkFramebuffer handle;
};

// TODO : dynamic framebuffer (handle with no VkFramebuffer backend)
