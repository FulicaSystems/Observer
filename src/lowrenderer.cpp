#include "vertex.hpp"
#include "vertexbuffer.hpp"
#include "shadermodule.hpp"
#include "graphicsdevice.hpp"

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
void ILowRenderer::destroy<class IShaderModule>(std::shared_ptr<class IShaderModule> ptr)
{
	destroyShaderModule_Impl(ptr);
}

template<>
std::shared_ptr<class ILogicalDevice> ILowRenderer::create<class ILogicalDevice>()
{
	return createLogicalDevice_Impl();
}
template<>
void ILowRenderer::destroy<class ILogicalDevice>(std::shared_ptr<class ILogicalDevice> ptr)
{
	destroyLogicalDevice_Impl(ptr);
}

template<>
std::shared_ptr<CommandBuffer> ILowRenderer::create<CommandBuffer>()
{
	return createCommandBuffer_Impl();
}