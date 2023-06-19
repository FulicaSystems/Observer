#include "vertexbufferdesc_gl.hpp"
#include "vertexbufferdesc_vk.hpp"

#include "vertexbuffer.hpp"

[[nodiscard]] std::shared_ptr<VertexBuffer> VertexBuffer::createNew(uint32_t vertexNum, const EGraphicsAPI graphicsApi)
{
	std::shared_ptr<VertexBuffer> vbo;
	switch (graphicsApi)
	{
	case EGraphicsAPI::OPENGL:
	{
		vbo = std::make_shared<VertexBuffer>(new VertexBufferDesc_Gl());
		break;
	}
	case EGraphicsAPI::VULKAN:
	{
		vbo = std::make_shared<VertexBuffer>(new VertexBufferDesc_Vk());
		break;
	}
	default:
		throw std::runtime_error("Invalid graphics API");
	}

	vbo->bufferSize = sizeof(Vertex) * (size_t)vertexNum;
	vbo->vertexNum = vertexNum;

	return vbo;
}