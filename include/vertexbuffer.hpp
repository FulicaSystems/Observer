#pragma once

#include <glad/vulkan.h>

struct MemoryBlock;

struct VertexBuffer
{
	// binded memory block
	MemoryBlock* memoryBlock = nullptr;
	size_t memoryOffset = 0;

	// CPU accessible data
	void* vertices;
	uint32_t vertexNum = 0;

	// buffer size
	size_t bufferSize = 0;
	// vertex buffer object
	VkBuffer buffer;
};