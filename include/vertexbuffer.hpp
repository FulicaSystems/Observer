#pragma once

#include <memory>

#include "vertexbufferdesc_vk.hpp"

// TODO : rename to VertexBuffer
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

	[[nodiscard]] static inline std::shared_ptr<VertexBuffer> createNew()
	{
		return std::make_shared<VertexBuffer>(new VertexBufferDesc_Vk());
	}
};