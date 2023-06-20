#include "utils/binary.hpp"
#include "utils/multithread/globalthreadpool.hpp"

#include "renderer.hpp"
#include "lowrenderer.hpp"

#include "graphicsdevice.hpp"
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
			local = highRenderer.api->create<IShaderModule>(highRenderer.device,
			vs.size(),
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
	highRenderer.api->destroy<IShaderModule>(std::dynamic_pointer_cast<IShaderModule>(local));
}