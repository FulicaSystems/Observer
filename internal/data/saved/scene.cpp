#include <vk_mem_alloc.h>

#include "context.hpp"
#include "resource_manager.hpp"

#include "device/memory/buffer.hpp"
#include "device/memory/descriptor.hpp"
#include "engine/uniform.hpp"

#include "scene.hpp"

void Scene::loadHost(const uint64_t index, const std::shared_ptr<ResourceLoadInfoT> loadInfo)
{
    auto r = std::make_shared<CPUScene>(index);
    hostResource = r;

    auto li = std::make_shared<MeshLoadInfoT>();
    li->deviceptr = loadInfo->deviceptr;
    li->vertices = {};
    r->m_meshes.emplace_back(ResourceManager::load<Mesh>(li));
}
void Scene::loadLocal(const std::shared_ptr<ResourceLoadInfoT> loadInfo)
{
    auto r = std::make_shared<GPUScene>();
    localResource = r;

    auto li = std::dynamic_pointer_cast<SceneLoadInfoT>(loadInfo);
    auto& device = li->deviceptr;
    const auto& cx = device->getContext();

    auto vertexShaderCreateInfo = std::make_shared<ShaderLoadInfoT>();
    vertexShaderCreateInfo->deviceptr = loadInfo->deviceptr;
    vertexShaderCreateInfo->filepath = "shaders/triangle.vert.spv";
    vertexShaderCreateInfo->stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertexShaderCreateInfo->entryPoint = "main";

    auto fragmentShaderCreateInfo = std::make_shared<ShaderLoadInfoT>();
    fragmentShaderCreateInfo->deviceptr = loadInfo->deviceptr;
    fragmentShaderCreateInfo->filepath = "shaders/triangle.frag.spv";
    fragmentShaderCreateInfo->stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragmentShaderCreateInfo->entryPoint = "main";

    auto descriptorCreateInfo = std::make_shared<UniformBufferCreateInfoT>();
    descriptorCreateInfo->devicePtr = loadInfo->deviceptr;
    descriptorCreateInfo->size = sizeof(UniformPerFrame);
    descriptorCreateInfo->setLayoutIndex = 0;
    descriptorCreateInfo->type = DescriptorTypeE::UNIFORM_BUFFER;

    r->m_renderStates
        .push_back(
            std::make_unique<RenderState>(
                RenderStateCreateInfoT{
                    .deviceptr = loadInfo->deviceptr,
                    .pipelineCreateInfo =
                        PipelineCreateInfoT{
                                            .device = loadInfo->deviceptr,
                                            .shaderStages =
                                {
                                    ResourceManager::load<Shader>(vertexShaderCreateInfo),
                                    ResourceManager::load<Shader>(fragmentShaderCreateInfo),
                                }, .vertexBindings =
                                {
                                    VkVertexInputBindingDescription{
                                        .binding = 0,
                                        .stride = sizeof(Vertex),
                                        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
                                    },
                                }, .vertexAttributes =
                                {
                                    VkVertexInputAttributeDescription{
                                        .location = 0,
                                        .binding = 0,
                                        .format = VK_FORMAT_R32G32B32_SFLOAT,
                                        .offset = offsetof(Vertex, position),
                                    },
                                    VkVertexInputAttributeDescription{
                                        .location = 1,
                                        .binding = 0,
                                        .format = VK_FORMAT_R32G32B32_SFLOAT,
                                        .offset = offsetof(Vertex, normal),
                                    },
                                    VkVertexInputAttributeDescription{
                                        .location = 2,
                                        .binding = 0,
                                        .format = VK_FORMAT_R32G32B32A32_SFLOAT,
                                        .offset = offsetof(Vertex, color),
                                    },
                                    VkVertexInputAttributeDescription{
                                        .location = 3,
                                        .binding = 0,
                                        .format = VK_FORMAT_R32G32_SFLOAT,
                                        .offset = offsetof(Vertex, uv),
                                    },
                                }, .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                                            .bPrimitiveRestartEnable = false,
                                            .viewportWidth = 1366,
                                            .viewportHeight = 768,
                                            .type = li->type,
                                            .setLayoutBindings =
                                {
                                    {
                                        VkDescriptorSetLayoutBinding{
                                            .binding = 0,
                                            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                            .descriptorCount = 1,
                                            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                                            .pImmutableSamplers = nullptr,
                                        },
                                    },
                                }, .poolSizes = {VkDescriptorPoolSize{
                                .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                .descriptorCount = 1,
                            }},
                                            .descriptorCreateInfos = {descriptorCreateInfo},
                                            .renderPass =
                                li && li->renderPass.has_value() ? li->renderPass.value() : nullptr,
                                            },
    }));

    auto host = std::static_pointer_cast<CPUScene>(hostResource);
    for (int i = 0; i < host->m_meshes.size(); ++i)
    {
        r->m_renderStates[0]->addObject(std::make_shared<MeshRenderDescription>(
            std::static_pointer_cast<GPUMesh>(host->m_meshes[i]->localResource)));
    }
}

void Scene::unloadHost()
{
}
void Scene::unloadLocal()
{
}
