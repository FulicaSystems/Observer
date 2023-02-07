#pragma once

#include <glad/vulkan.h>

struct VertexBuffer
{
	// CPU accessible data
	void* vertices;
	uint32_t vertexNum = 0;

	// buffer size
	size_t bufferSize = 0;
	// vertex buffer object
	VkBuffer buffer;

	// memory offset of this buffer object
	uint32_t offset = 0;
	// memory allocated on the GPU heap
	VkDeviceMemory memory;
};