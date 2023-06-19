#pragma once

#include <glad/vulkan.h>

#include "memorymanager.hpp"

#include "vertexbuffer.hpp"

class VertexBufferDesc_Vk : public IVertexBufferLocalDesc
{
public:
	class IAllocation* alloc;

	// vertex buffer object
	VkBuffer buffer;

	VertexBufferDesc_Vk()
	{
#ifdef USE_VMA
		alloc = new Alloc_VMA();
#else
		alloc = new MyAlloc_Vk();
#endif
	}
	~VertexBufferDesc_Vk() override { delete alloc; }
};