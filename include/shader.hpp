#pragma once

#include <vector>
#include <memory>
#include <array>

#include <glad/vulkan.h>

#include "utils/derived.hpp"

#include "resource.hpp"

class Shader : public IHostResource
{
	SUPER(IHostResource)

private:

public:
	std::vector<char> vs;
	std::vector<char> fs;

	~Shader() override
	{
		gpuUnload();
		cpuUnload();
	}

	void cpuLoad() override;
	void gpuLoad() override;

	void cpuUnload() override;
	void gpuUnload() override;
};

class ShaderModule

{
private:

public:
	VkShaderModule vsModule = nullptr;
	VkShaderModule fsModule = nullptr;

	std::array<VkPipelineShaderStageCreateInfo, 2> getCreateInfo();

};


template<>
struct Local<ShaderModule>
{

};