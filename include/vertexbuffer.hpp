#pragma once

#include <memory>

#include <glad/vulkan.h>

#include "memorymanager.hpp"

#ifdef USE_VMA
#include "vmahelper.hpp"
#else
#include "allocator.hpp"
#endif

// TODO : rename to VertexBuffer
class temp
{
public:
	// CPU accessible data
	void* vertices = 0;
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
#ifdef USE_VMA
		return std::make_shared<VertexBuffer>(new VMAHelperAlloc());
#else
		return std::make_shared<VertexBuffer>(new MyAlloc());
#endif
	}
};