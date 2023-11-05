#include "utils/binary.hpp"

#include "device.hpp"

#include "shader.hpp"


void Shader::load()
{
	std::string str = filepath.string();
	vs = bin::read(str + ".vert.spv");
	fs = bin::read(str + ".frag.spv");

	loaded.test_and_set();
}

void Shader::unload()
{
	vs.clear();
	fs.clear();
}

void GPUShader::load()
{
	auto base = (Shader*)host;

	// vertex shader module
	VkShaderModuleCreateInfo vsModuleCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.codeSize = base->vs.size(),
		.pCode = reinterpret_cast<const uint32_t*>(base->vs.data()),
	};
	vsModule = device.create<ShaderModule>(&vsModuleCreateInfo);
	// fragment shader module
	VkShaderModuleCreateInfo fsModuleCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.codeSize = base->fs.size(),
		.pCode = reinterpret_cast<const uint32_t*>(base->fs.data()),
	};
	fsModule = device.create<ShaderModule>(&fsModuleCreateInfo);

	loaded.test_and_set();
	loaded.notify_all();
}

void GPUShader::unload()
{
	device.destroy<ShaderModule>(vsModule);
	device.destroy<ShaderModule>(fsModule);
}