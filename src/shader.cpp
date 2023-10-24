#include "utils/binary.hpp"

#include "device.hpp"

#include "shader.hpp"


void Shader::cpuLoad()
{
	std::string str = filepath.string();
	vs = bin::read(str + ".vert.spv");
	fs = bin::read(str + ".frag.spv");
}

void Shader::gpuLoad()
{
	auto asset = std::make_shared<GPUShader>(vs.size(),
		fs.size(),
		vs.data(),
		fs.data());
	asset->vsModule = device.create<ShaderModule>();
	asset->fsModule = device.create<ShaderModule>();

	local = asset;
	loaded.test_and_set();
	loaded.notify_all();
}

void Shader::cpuUnload()
{
	vs.clear();
	fs.clear();
}

void Shader::gpuUnload()
{
	device.destroy<ShaderModule>(((GPUShader*)local.get())->vsModule);
	device.destroy<ShaderModule>(((GPUShader*)local.get())->fsModule);
}