#pragma once

#include <cstdint>

#include "utils/derived.hpp"
#include "lowgraphicsobject.hpp"

// comment this macro definition to use a custom memory allocator
#define USE_VMA // define this macro to use Vulkan Memory Allocator

class IAllocation
{
};

class IMemoryAllocator : public ILowGraphicsObject
{
	SUPER(ILowGraphicsObject)

protected:
	virtual void createAllocatorInstance() = 0;
	virtual void destroyAllocatorInstance() = 0;

public:
	void create(ILowRenderer* api, ILogicalDevice* device) override { Super::create(api, device); createAllocatorInstance(); };
	void destroy() override { destroyAllocatorInstance(); };

	virtual void allocateBufferObjectMemory(class VkBufferCreateInfo& createInfo,
		size_t bufferSize,
		class IVertexBufferLocalDesc* desc,
		uint32_t memoryFlags = 0,
		bool mappable = false) = 0;
	virtual void destroyBufferObjectMemory(class IVertexBufferLocalDesc* desc, size_t bufferSize) = 0;

	virtual void mapMemory(IAllocation* allocation, void** ppData) = 0;
	virtual void unmapMemory(IAllocation* allocation) = 0;
};

#ifdef USE_VMA
#include "vmahelper.hpp"
#else
#include "allocator_vk.hpp"
#endif