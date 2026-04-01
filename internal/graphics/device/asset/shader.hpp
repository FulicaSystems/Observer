#pragma once

#include <array>
#include <memory>
#include <vector>

#include <vulkan/vulkan.h>

#include "data/resource.hpp"

struct ShaderCreateInfoT
{
    std::vector<char> source;
    VkShaderStageFlagBits stage;
    const char* entryPoint;
};

struct ShaderLoadInfoT : public ResourceLoadInfoT, public ShaderCreateInfoT
{
    std::size_t hash() const override
    {
        std::size_t h1 = std::hash<const char*>{}(filepath.string().c_str());
        std::size_t h2 = std::hash<const char*>{}(source.data());
        return h1 + h2;
    }
};

class Shader : public ResourceABC
{
  public:
    void loadHost(const uint64_t index, const ResourceLoadInfoT* loadInfo) override;
    void loadLocal(const ResourceLoadInfoT* loadInfo) override;

    void unloadHost() override;
    void unloadLocal() override;
};

class CPUShader : public HostResourceABC
{
  public:
    CPUShader(uint64_t index) : HostResourceABC(index) {}
    std::vector<char> source;
};

class GPUShader : public LocalResourceABC
{
  public:
    VkShaderModule module;
    VkPipelineShaderStageCreateInfo createInfo;
};