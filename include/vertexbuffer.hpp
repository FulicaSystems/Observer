#pragma once

#include <glad/vulkan.h>

#include <vk_mem_alloc.h>

struct MemoryBlock;

struct VertexBuffer
{
#if false
	// binded memory block
	MemoryBlock* memoryBlock = nullptr;
	size_t memoryOffset = 0;
#else
	VmaAllocation allocation;
#endif

	// CPU accessible data
	void* vertices;
	uint32_t vertexNum = 0;

	// buffer size
	size_t bufferSize = 0;
	// vertex buffer object
	VkBuffer buffer;
};