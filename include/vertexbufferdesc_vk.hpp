#pragma once

#include <glad/vulkan.h>

#include "vertexbufferlocaldesc.hpp"
#include "memorymanager.hpp"

#ifdef USE_VMA
#include "vmahelper.hpp"
#else
#include "allocator_vk.hpp"
#endif

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
	~VertexBufferDesc_Vk() { delete alloc; }
};