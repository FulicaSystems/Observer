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
	VkShaderModule vsModule = nullptr;
	VkShaderModule fsModule = nullptr;

	std::array<VkPipelineShaderStageCreateInfo, 2> getCreateInfo();
};