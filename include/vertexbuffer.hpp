#pragma once

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
private:
	VertexBuffer(class IAllocation* allocation) : alloc(allocation) {}

public:
	class IAllocation* alloc;

	// vertex buffer object
	VkBuffer buffer;

	~VertexBuffer() { delete alloc; }

	static inline VertexBuffer createNew()
	{
		return VertexBuffer(new VMAHelperAlloc());
	}
};