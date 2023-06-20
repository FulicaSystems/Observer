#pragma once

#include <glad/vulkan.h>

#include "memorymanager.hpp"

#include "vertexbuffer.hpp"

class VertexBuffer_Vk : public IVertexBuffer
{
public:
	class IAllocation* alloc;

	// vertex buffer object
	VkBuffer buffer;

	VertexBuffer_Vk()
	{
#ifdef USE_VMA
		alloc = new Alloc_VMA();
#else
		alloc = new MyAlloc_Vk();
#endif
	}
	~VertexBuffer_Vk() override { delete alloc; }
};