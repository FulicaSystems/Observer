#pragma once

#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

#include <vulkan/vulkan.h>

#include "render_pass.hpp"

#include "shader.hpp"

enum class PipelineTypeE
{
    /**
     * @brief rasterization pipeline (can be used with ray queries)
     *
     */
    GRAPHICS = 0,
    RASTER = GRAPHICS,
    /**
     * @brief compute pipeline
     *
     */
    COMPUTE = 1,
    /**
     * @brief ray tracing pipeline
     * TODO
     *
     */
    RAYTRACE = 2,

    COUNT = 3,
};

struct PipelineCreateInfoT
{
    std::vector<std::shared_ptr<Shader>> shaderStages;

    std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT,
                                                 VK_DYNAMIC_STATE_SCISSOR};

    std::vector<VkVertexInputBindingDescription> vertexBindings;
    std::vector<VkVertexInputAttributeDescription> vertexAttributes;

    VkPrimitiveTopology topology;
    bool bPrimitiveRestartEnable;

    uint32_t viewportWidth;
    uint32_t viewportHeight;

    VkPipelineRasterizationStateCreateInfo rasterizerCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .cullMode = VK_CULL_MODE_BACK_BIT,
        .frontFace = VK_FRONT_FACE_CLOCKWISE,
        .depthBiasEnable = VK_FALSE,
        .depthBiasConstantFactor = 0.f,
        .depthBiasClamp = 0.f,
        .depthBiasSlopeFactor = 0.f,
        .lineWidth = 1.f,
    };

    VkPipelineMultisampleStateCreateInfo multisamplingCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .sampleShadingEnable = VK_FALSE,
        .minSampleShading = 1.f,
        .pSampleMask = nullptr,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable = VK_FALSE,
    };

    VkPipelineDepthStencilStateCreateInfo depthStencilCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .depthTestEnable = VK_TRUE,
        .depthWriteEnable = VK_TRUE,
        .depthCompareOp = VK_COMPARE_OP_LESS,
        .depthBoundsTestEnable = VK_FALSE,
        .stencilTestEnable = VK_FALSE,
        .front = {},
        .back = {},
        .minDepthBounds = 0.0f,
        .maxDepthBounds = 1.0f,
    };

    std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachment = {
        {
         .blendEnable = VK_TRUE,
         .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
         .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
         .colorBlendOp = VK_BLEND_OP_ADD,
         .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
         .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
         .alphaBlendOp = VK_BLEND_OP_ADD,
         .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                              VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
         }
    };
    float blendConstants[4] = {0.f, 0.f, 0.f, 0.f};

    /**
     * @brief different set layout bindings may be used in different set layout
     *
     */
    std::vector<std::vector<VkDescriptorSetLayoutBinding>> setLayoutBindings;
    std::vector<VkPushConstantRange> pushConstantRanges;

    std::shared_ptr<RenderPass> renderPass;
    uint32_t subpassIndex = 0;
};

class Pipeline
{
  private:
    PipelineCreateInfoT ci;

    // shaders containing all the pipeline stage information (vertex shader stage, fragment shader
    // stage, ray tracing, ...)
    std::vector<std::shared_ptr<Shader>> shaderProgram;

    VkDescriptorSetLayout setlayout;
    std::vector<VkDescriptorSet> sets;

    VkPipelineLayout m_layout;

    VkPipeline m_handle;

  public:
    Pipeline() = delete;
    Pipeline(PipelineCreateInfoT ci) : ci(ci) {}

    ~Pipeline()
    {
        // TODO
    }

  public:
    [[nodiscard]] VkPipelineLayout& getLayoutHandle() { return m_layout; }

    [[nodiscard]] VkPipeline& getHandle() { return m_handle; }
    [[nodiscard]] const VkPipeline& getHandle() const { return m_handle; }
};