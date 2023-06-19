#include "vertex.hpp"
#include "vertexbuffer.hpp"
#include "shadermodule.hpp"
#include "graphicsdevice.hpp"

#include "lowrenderer.hpp"

template<>
std::shared_ptr<VertexBuffer> ILowRenderer::create<VertexBuffer,
	uint32_t,
	const Vertex*>(uint32_t vertexNum,
		const Vertex* vertices)
{
	return createVertexBuffer_Impl(vertexNum, vertices);
}

template<>
std::shared_ptr<ShaderModule> ILowRenderer::create<ShaderModule,
	ILogicalDevice*,
	size_t,
	size_t,
	char*,
	char*>(ILogicalDevice* device,
		size_t vsSize,
		size_t fsSize,
		char* vs,
		char* fs)
{
	return createShaderModule_Impl(device, vsSize, fsSize, vs, fs);
}

template<>
std::shared_ptr<CommandBuffer> ILowRenderer::create<CommandBuffer>()
{
	return createCommandBuffer_Impl();
}