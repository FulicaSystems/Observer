#include "resource_manager.hpp"

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

    auto host = std::static_pointer_cast<CPUScene>(hostResource);
    r->m_meshRenderStates.reserve(host->m_meshes.size());
    for (int i = 0; i < host->m_meshes.size(); ++i)
    {
        auto createInfo = std::make_shared<MeshRenderDescriptionCreateInfoT>();
        createInfo->deviceptr = loadInfo->deviceptr;

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

        createInfo->pipelineCreateInfo = PipelineCreateInfoT{
            .shaderStages =
                {
                               ResourceManager::load<Shader>(vertexShaderCreateInfo),
                               ResourceManager::load<Shader>(fragmentShaderCreateInfo),
                               },
            .vertexBindings =
                {
                               VkVertexInputBindingDescription{
                        .binding = 0,
                        .stride = sizeof(Vertex),
                        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
                    }, },
            .vertexAttributes =
                {

                               VkVertexInputAttributeDescription{
                        .location = 0,
                        .binding = 0,
                        .format = VK_FORMAT_R32G32B32_SFLOAT,
                        .offset = offsetof(Vertex, position),
                    }, VkVertexInputAttributeDescription{
                        .location = 1,
                        .binding = 0,
                        .format = VK_FORMAT_R32G32B32_SFLOAT,
                        .offset = offsetof(Vertex, normal),
                    }, VkVertexInputAttributeDescription{
                        .location = 2,
                        .binding = 0,
                        .format = VK_FORMAT_R32G32B32A32_SFLOAT,
                        .offset = offsetof(Vertex, color),
                    }, VkVertexInputAttributeDescription{
                        .location = 3,
                        .binding = 0,
                        .format = VK_FORMAT_R32G32_SFLOAT,
                        .offset = offsetof(Vertex, uv),
                    }, },
            .viewportWidth = 1366,
            .viewportHeight = 768,
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
                    }, },
            .renderPass = li && li->renderPass.has_value() ? li->renderPass.value() : nullptr,
        };
        createInfo->meshView = std::static_pointer_cast<GPUMesh>(host->m_meshes[i]->localResource);
        r->m_meshRenderStates.emplace_back(std::make_shared<MeshRenderDescription>(createInfo));
    }
}

void Scene::unloadHost()
{
}
void Scene::unloadLocal()
{
}
