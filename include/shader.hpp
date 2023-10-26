#pragma once

#include <vector>
#include <memory>
#include <array>

#include <glad/vulkan.h>

#include "utils/derived.hpp"

#include "resource.hpp"

class Shader : public HostResourceABC
{
	SUPER(HostResourceABC)

public:
	std::vector<char> vs;
	std::vector<char> fs;


	void load() override;
	void unload() override;
};



// TODO : remove include
#include "shadermodule.hpp"

class GPUShader : public LocalResourceABC
{
private:

public:
	// TODO : vector of modules for multiple pass handling
	std::shared_ptr<ShaderModule> vsModule = nullptr;
	std::shared_ptr<ShaderModule> fsModule = nullptr;

	std::array<VkPipelineShaderStageCreateInfo, 2> getCreateInfo()
	{
		VkPipelineShaderStageCreateInfo vsStageCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.stage = VK_SHADER_STAGE_VERTEX_BIT,
			.module = vsModule->handle,
			.pName = "main",
			.pSpecializationInfo = nullptr
		};

		VkPipelineShaderStageCreateInfo fsStageCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
			.module = fsModule->handle,
			.pName = "main",
			.pSpecializationInfo = nullptr
		};

		std::array<VkPipelineShaderStageCreateInfo, 2> createInfo;
		createInfo[0] = vsStageCreateInfo;
		createInfo[1] = fsStageCreateInfo;
		return createInfo;
	}

	void load() override;
	void unload() override;
};