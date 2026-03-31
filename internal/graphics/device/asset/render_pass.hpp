#pragma once

#include <utility>
#include <vector>

#include <vulkan/vulkan.h>

using Attachment = std::pair<VkAttachmentDescription, VkAttachmentReference>;

struct SubpassDescriptionT
{
    VkPipelineBindPoint pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    std::vector<uint32_t> colorAttachmentIndices;
    bool bDepthAttachment;
};

struct RenderPassCreateInfoT
{
    std::vector<Attachment> colorAttachments = {
        {{
             .samples = VK_SAMPLE_COUNT_1_BIT,
             // load : what to do with the already existing image on the framebuffer
             .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
             // store : what to do with the newly rendered image on the framebuffer
             .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
             .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
             .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
             .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
             .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
         }, {
             .attachment = 0,
             .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
         }},
    };

    Attachment depthAttachment = {
        {
         .samples = VK_SAMPLE_COUNT_1_BIT,
         .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
         .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
         .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
         .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
         .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
         .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
         },
        {
         .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
         }
    };

    std::vector<SubpassDescriptionT> subpasses = {
        {
         .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
         }
    };

    std::vector<VkSubpassDependency> dependencies = {
        {
         .srcSubpass = VK_SUBPASS_EXTERNAL,
         .dstSubpass = 0,
         .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, .srcAccessMask = 0,
         .dstAccessMask =
                VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
         }
    };
};

class RenderPass
{
  public:
    VkRenderPass handle;
};