#include <stdexcept>

#include "vertexbuffer_gl.hpp"
#include "vertexbuffer_vk.hpp"

#include "vertex.hpp"
#include "vertexbuffer.hpp"

[[nodiscard]] std::shared_ptr<IVertexBuffer> IVertexBuffer::instantiate(uint32_t vertexNum, const EGraphicsAPI graphicsApi)
{
	std::shared_ptr<IVertexBuffer> vbo;
	switch (graphicsApi)
	{
	case EGraphicsAPI::OPENGL:
	{
		vbo = std::make_shared<VertexBuffer_Gl>();
		break;
	}
	case EGraphicsAPI::VULKAN:
	{
		vbo = std::make_shared<VertexBuffer_Vk>();
		break;
	}
	default:
		throw std::runtime_error("Invalid graphics API");
	}

	vbo->bufferSize = sizeof(Vertex) * (size_t)vertexNum;
	vbo->vertexNum = vertexNum;

	return vbo;
}