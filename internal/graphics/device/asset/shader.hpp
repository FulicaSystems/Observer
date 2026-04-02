#pragma once

#include <array>
#include <memory>
#include <vector>

#include <vulkan/vulkan.h>

#include "data/resource.hpp"

struct ShaderCreateInfoT
{
    /**
     * @brief source can be specified but stays optional since it will be read from the .spv given
     * its filepath
     *
     */
    std::optional<std::vector<char>> source;
    VkShaderStageFlagBits stage;
    const char* entryPoint;
};

struct ShaderLoadInfoT : public ResourceLoadInfoT, public ShaderCreateInfoT
{
    std::size_t hash() const override
    {
        std::size_t h1 = -2ULL;
        if (filepath.has_value())
            h1 = std::hash<const char*>{}(filepath.value().string().c_str());
        std::size_t h2 = -1ULL;
        if (source.has_value())
            h2 = std::hash<const char*>{}(source.value().data());
        return h1 + h2;
    }
};

class Shader : public ResourceABC
{
  public:
    void loadHost(const uint64_t index, const std::shared_ptr<ResourceLoadInfoT> loadInfo) override;
    void loadLocal(const std::shared_ptr<ResourceLoadInfoT> loadInfo) override;

    void unloadHost() override;
    void unloadLocal() override;
};

class CPUShader : public HostResourceABC
{
  public:
    std::vector<char> source;

    CPUShader() = delete;
    CPUShader(uint64_t index) : HostResourceABC(index) {}
};

class GPUShader : public LocalResourceABC
{
  public:
    VkShaderModule module;
    VkPipelineShaderStageCreateInfo createInfo;
};
