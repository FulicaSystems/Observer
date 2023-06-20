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

const IShaderModule* Shader::getModule() const
{
	return ((ShaderRenderer*)local)->shmodule.get();
}

#include "lowrenderer.hpp"
void ShaderRenderer::create(IHostResource* host)
{
	Shader* hostResource = (Shader*)host;

	Utils::GlobalThreadPool::addTask([=, this]() {
		shmodule = highRenderer.api->create<IShaderModule>(highRenderer.device,
			hostResource->vs.size(),
			hostResource->fs.size(),
			hostResource->vs.data(),
			hostResource->fs.data());
		hostResource->loaded.test_and_set();
		hostResource->loaded.notify_all();
		}, false);
}

void ShaderRenderer::destroy(class IHostResource* host)
{
	shmodule.reset();
}