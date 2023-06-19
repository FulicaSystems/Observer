#include "utils/binary.hpp"
#include "utils/multithread/globalthreadpool.hpp"

#include "renderer.hpp"
#include "graphicsdevice.hpp"
#include "shadermodule.hpp"
#include "shader.hpp"

void Shader::cpuLoad()
{
	std::string str = filepath.string();
	vs = bin::read(str + ".vert.spv");
	fs = bin::read(str + ".frag.spv");
}

void Shader::cpuUnload()
{
	vs.clear();
	fs.clear();
}

const ShaderModule* Shader::getModule() const
{
	return ((ShaderCompiled*)local)->shmodule.get();
}

#include "lowrenderer.hpp"
void ShaderCompiled::create(IHostResource* host)
{
	Shader* hostResource = (Shader*)host;

	Utils::GlobalThreadPool::addTask([=, this]() {
		shmodule = highRenderer.api->create<ShaderModule>(highRenderer.device,
			hostResource->vs.size(),
			hostResource->fs.size(),
			hostResource->vs.data(),
			hostResource->fs.data());
		hostResource->loaded.test_and_set();
		hostResource->loaded.notify_all();
		}, false);
}

void ShaderCompiled::destroy(class IHostResource* host)
{
	shmodule.reset();
}