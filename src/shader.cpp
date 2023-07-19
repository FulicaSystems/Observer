#include "utils/binary.hpp"
#include "utils/multithread/globalthreadpool.hpp"

#include "lowrenderer.hpp"

#include "shadermodule.hpp"

#include "shader.hpp"

void Shader::cpuLoad()
{
	std::string str = filepath.string();
	vs = bin::read(str + ".vert.spv");
	fs = bin::read(str + ".frag.spv");
}

void Shader::gpuLoad()
{
	Utils::GlobalThreadPool::addTask([=, this]() {
			local = lowrdr.create<IShaderModule>(vs.size(),
			fs.size(),
			vs.data(),
			fs.data());
		loaded.test_and_set();
		loaded.notify_all();
		}, false);
}

void Shader::cpuUnload()
{
	vs.clear();
	fs.clear();
}

void Shader::gpuUnload()
{
	lowrdr.destroy<IShaderModule>(std::dynamic_pointer_cast<IShaderModule>(local));
}