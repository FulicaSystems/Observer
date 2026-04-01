#pragma once

#include <array>
#include <memory>
#include <vector>

#include <vulkan/vulkan.h>

#include "data/resource.hpp"

struct ShaderLoadInfoT : public ResourceLoadInfoT
{
    std::vector<char> source;
    VkShaderStageFlagBits stage;
    const char* entryPoint;

} typedef ShaderCreateInfoT;

class Shader : public ResourceABC
{
  public:
    void loadHost(const uint64_t index, const ResourceLoadInfoT& loadInfo) override;
    void loadLocal(const ResourceLoadInfoT& loadInfo) override;

    void unloadHost(const ResourceLoadInfoT& loadInfo) override;
    void unloadLocal(const ResourceLoadInfoT& loadInfo) override;
};

class CPUShader : public HostResourceABC
{
  public:
    std::vector<char> source;
};

class GPUShader : public LocalResourceABC
{
  public:
    VkShaderModule module;
};