#include "vertex.hpp"
#include "vertexbuffer.hpp"
#include "shadermodule.hpp"

#include "lowrenderer.hpp"

template<>
std::shared_ptr<IVertexBuffer> ILowRenderer::create<IVertexBuffer,
	uint32_t,
	const Vertex*>(uint32_t vertexNum,
		const Vertex* vertices)
{
	return createVertexBuffer_Impl(vertexNum, vertices);
}

template<>
std::shared_ptr<IShaderModule> ILowRenderer::create<IShaderModule,
	size_t,
	size_t,
	char*,
	char*>(size_t vsSize,
		size_t fsSize,
		char* vs,
		char* fs)
{
	return createShaderModule_Impl(vsSize, fsSize, vs, fs);
}
template<>
void ILowRenderer::destroy<IShaderModule>(std::shared_ptr<IShaderModule> ptr)
{
	destroyShaderModule_Impl(ptr);
}

template<>
std::shared_ptr<ICommandBuffer> ILowRenderer::create<ICommandBuffer>()
{
	return createCommandBuffer_Impl();
}