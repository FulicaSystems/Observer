#pragma once

#include <memory>
#include <filesystem>
#include <array>

#include <glad/vulkan.h>

#include "shadermodule.hpp"

class ShaderModule_Vk : public IShaderModule
{
private:

public:
	class LogicalDevice_Vk* device = nullptr;

	VkShaderModule vsModule = nullptr;
	VkShaderModule fsModule = nullptr;

	~ShaderModule_Vk() override;

	std::array<VkPipelineShaderStageCreateInfo, 2> getCreateInfo();
};