#pragma once

#include <memory>
#include <stdexcept>

#include "vertex.hpp"

#include "graphicsapi.hpp"

// local (gpu) vertex buffer interface
// override with different graphics API
class IVertexBufferLocalDesc
{
public:
	virtual ~IVertexBufferLocalDesc() {}
};

class VertexBuffer
{
public:
	// CPU accessible data
	void* vertices = 0;
	uint32_t vertexNum = 0;

	// buffer size
	size_t bufferSize = 0;

public:
	IVertexBufferLocalDesc* localDesc;

	explicit VertexBuffer(IVertexBufferLocalDesc* localDesc) : localDesc(localDesc) {}
	~VertexBuffer() { delete localDesc; }

	[[nodiscard]] static std::shared_ptr<VertexBuffer> createNew(uint32_t vertexNum, const EGraphicsAPI graphicsApi = EGraphicsAPI::VULKAN);
};