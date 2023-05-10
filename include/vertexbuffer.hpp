#pragma once

#include <memory>

#include <glad/vulkan.h>

#include "allocator.hpp"
#include "vmahelper.hpp"

// TODO : rename to VertexBuffer
class temp
{
public:
	// CPU accessible data
	void* vertices;
	uint32_t vertexNum = 0;

	// buffer size
	size_t bufferSize = 0;
};

// TODO : rename to VertexBufferVk
class VertexBuffer : public temp
{
public:
	class IAllocation* alloc;

	// vertex buffer object
	VkBuffer buffer;

	explicit VertexBuffer(class IAllocation* allocation) : alloc(allocation) {}
	~VertexBuffer() { delete alloc; }

	[[nodiscard]] static inline std::shared_ptr<VertexBuffer> createNew()
	{
		return std::make_shared<VertexBuffer>(new MyAlloc());
	}
};