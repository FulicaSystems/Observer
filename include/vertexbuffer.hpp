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

	// memory allocated on the GPU heap
	VkDeviceMemory memory;
};