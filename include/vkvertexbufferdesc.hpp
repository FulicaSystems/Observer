#pragma once

#include <glad/vulkan.h>

#include "vertexbufferlocaldesc.hpp"
#include "memorymanager.hpp"

#ifdef USE_VMA
#include "vmahelper.hpp"
#else
#include "allocator.hpp"
#endif

class VkVertexBufferDesc : public IVertexBufferLocalDesc
{
public:
	class IAllocation* alloc;

	// vertex buffer object
	VkBuffer buffer;

	VkVertexBufferDesc()
	{
#ifdef USE_VMA
		alloc = new VMAHelperAlloc();
#else
		alloc = new MyAlloc();
#endif
	}
	~VkVertexBufferDesc() { delete alloc; }
};