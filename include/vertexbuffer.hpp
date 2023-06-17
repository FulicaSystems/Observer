#pragma once

#include <memory>

#include "vertex.hpp"

#include "graphicsapi.hpp"
#include "vertexbufferdesc_gl.hpp"
#include "vertexbufferdesc_vk.hpp"

class VertexBuffer
{
public:
	// CPU accessible data
	void* vertices = 0;
	uint32_t vertexNum = 0;

	// buffer size
	size_t bufferSize = 0;

	class IVertexBufferLocalDesc* localDesc;

	explicit VertexBuffer(class IVertexBufferLocalDesc* localDesc) : localDesc(localDesc) {}
	~VertexBuffer() { delete localDesc; }

	[[nodiscard]] static inline std::shared_ptr<VertexBuffer> createNew(uint32_t vertexNum, const EGraphicsAPI graphicsApi = EGraphicsAPI::VULKAN)
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
};