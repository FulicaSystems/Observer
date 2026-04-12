#pragma once

#include <iostream>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include <vulkan/vulkan.h>

#include "render_pass.hpp"

#include "shader.hpp"

class LogicalDevice;
struct DescriptorCreateInfoT;
class DescriptorABC;
enum class DescriptorFrequencyE;
class UniformBuffer;

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

enum class BufferingTypeE;

struct PipelineCreateInfoT
{
    const LogicalDevice* device;

    std::vector<std::shared_ptr<Shader>> shaderStages;

    std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT,
                                                 VK_DYNAMIC_STATE_SCISSOR};

    std::vector<VkVertexInputBindingDescription> vertexBindings;
    std::vector<VkVertexInputAttributeDescription> vertexAttributes;

    VkPrimitiveTopology topology;
    /**
     * @brief
     * https://docs.vulkan.org/refpages/latest/refpages/source/VkPipelineInputAssemblyStateCreateInfo.html#_members
     *
     */
    bool bPrimitiveRestartEnable = false;

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

    BufferingTypeE type;

    struct DescriptorSetDescriptionT
    {
        DescriptorFrequencyE frequency;
        std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings;
    };
    std::vector<DescriptorSetDescriptionT> setDescriptions;

    std::vector<VkDescriptorPoolSize> poolSizes;
    std::vector<VkPushConstantRange> pushConstantRanges;

    const RenderPass* renderPass;
    uint32_t subpassIndex = 0;
};

class DescriptorBlock
{
  public:
    VkDescriptorPool pool;

    std::unordered_map<DescriptorFrequencyE, std::vector<VkDescriptorSet>> sets;
};

class Pipeline
{
  private:
    PipelineCreateInfoT ci;

    /**
     * @brief shaders containing all the pipeline stage information (vertex shader stage, fragment
     * shader stage, ray tracing, ...)
     *
     */
    std::vector<std::shared_ptr<Shader>> shaderProgram;

    /**
     * @brief array (element per back buffer)
     *
     */
    std::vector<std::unique_ptr<DescriptorBlock>> m_descriptorBlocks;
    std::vector<VkDescriptorSetLayout> m_setLayouts;
    VkPipelineLayout m_layout;

    VkPipeline m_handle;

  public:
    Pipeline() = delete;
    Pipeline(PipelineCreateInfoT ci) : ci(ci) {}

    ~Pipeline()
    {
        // TODO
    }

    void recreateDescriptorSets(const BufferingTypeE& type);
    // TODO : do other types of descriptors
    void writeDescriptorSets(const DescriptorFrequencyE frequency, const uint32_t setIndex,
                             const UniformBuffer& ubo) const;

  public:
    [[nodiscard]] std::vector<VkDescriptorSetLayout>& getSetLayouts() { return m_setLayouts; }
    [[nodiscard]] const std::vector<VkDescriptorSetLayout>& getSetLayouts() const
    {
        return m_setLayouts;
    }
    [[nodiscard]] VkPipelineLayout& getLayoutHandle() { return m_layout; }
    [[nodiscard]] const VkPipelineLayout& getLayoutHandle() const { return m_layout; }

    [[nodiscard]] VkPipeline& getHandle() { return m_handle; }
    [[nodiscard]] const VkPipeline& getHandle() const { return m_handle; }

    [[nodiscard]] const std::vector<VkDescriptorSet>& getDescriptorSetHandles(
        uint32_t backBufferIndex, const DescriptorFrequencyE type) const
    {
        return m_descriptorBlocks[backBufferIndex]->sets[type];
    }
};
