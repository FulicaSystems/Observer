#pragma once

#include <cstdint>

#include "utils/derived.hpp"

// comment this macro definition to use a custom memory allocator
#define USE_VMA // define this macro to use Vulkan Memory Allocator

class IAllocation
{
public:
	virtual ~IAllocation() {}
};

class IMemoryAllocator
{
protected:
	virtual void createAllocatorInstance() = 0;
	virtual void destroyAllocatorInstance() = 0;

public:
	IMemoryAllocator() { createAllocatorInstance(); };
	virtual ~IMemoryAllocator() { destroyAllocatorInstance(); };

	virtual void allocateBufferObjectMemory(class VkBufferCreateInfo& createInfo,
		class IVertexBuffer* vbo,
		uint32_t memoryFlags = 0,
		bool mappable = false) = 0;
	virtual void destroyBufferObjectMemory(class IVertexBuffer* vbo) = 0;

	virtual void mapMemory(IAllocation* allocation, void** ppData) = 0;
	virtual void unmapMemory(IAllocation* allocation) = 0;
};

#ifdef USE_VMA
#include "vmahelper.hpp"
#else
#include "allocator_vk.hpp"
#endif