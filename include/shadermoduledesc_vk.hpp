#pragma once

#include <memory>
#include <filesystem>
#include <array>

#include <glad/vulkan.h>

#include "shadermodule.hpp"

class ShaderModuleDesc_Vk : public IShaderModuleLocalDesc
{
private:

public:
	class LogicalDevice_Vk* device = nullptr;

	VkShaderModule vsModule = nullptr;
	VkShaderModule fsModule = nullptr;

	~ShaderModuleDesc_Vk() override;

	std::array<VkPipelineShaderStageCreateInfo, 2> getCreateInfo();
};