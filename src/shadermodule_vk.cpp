#include "utils/binary.hpp"

#include "graphicsdevice_vk.hpp"
#include "shadermodule_vk.hpp"

ShaderModule_Vk::~ShaderModule_Vk()
{
	vkDestroyShaderModule(device->vkdevice, vsModule, nullptr);
	vkDestroyShaderModule(device->vkdevice, fsModule, nullptr);
}

std::array<VkPipelineShaderStageCreateInfo, 2> ShaderModule_Vk::getCreateInfo()
{
	VkPipelineShaderStageCreateInfo vsStageCreateInfo = {
	.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
	.stage = VK_SHADER_STAGE_VERTEX_BIT,
	.module = vsModule,
	.pName = "main",
	//for shader constants values
	.pSpecializationInfo = nullptr
	};

	VkPipelineShaderStageCreateInfo fsStageCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
		.module = fsModule,
		.pName = "main",
		.pSpecializationInfo = nullptr
	};

	std::array<VkPipelineShaderStageCreateInfo, 2> createInfo;
	createInfo[0] = vsStageCreateInfo;
	createInfo[1] = fsStageCreateInfo;
	return createInfo;
}