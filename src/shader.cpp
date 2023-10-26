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
	auto asset = std::make_shared<GPUShader>(base->vs.size(),
		base->fs.size(),
		base->vs.data(),
		base->fs.data());
	asset->vsModule = device.create<ShaderModule>();
	asset->fsModule = device.create<ShaderModule>();

	loaded.test_and_set();
	loaded.notify_all();
}

void GPUShader::unload()
{
	device.destroy<ShaderModule>(vsModule);
	device.destroy<ShaderModule>(fsModule);
}