#pragma once

#include "graphicsobject.hpp"

// comment this macro definition to use a custom memory allocator
#define USE_VMA // define this macro to use Vulkan Memory Allocator

class IAllocation
{
};

class IMemoryAllocator : public IGraphicsObject
{
	SUPER(IGraphicsObject)

protected:
	virtual void createAllocatorInstance() = 0;
	virtual void destroyAllocatorInstance() = 0;

public:
	void create(LowRenderer* api, LogicalDevice* device) override { Super::create(api, device); createAllocatorInstance(); };
	void destroy() override { destroyAllocatorInstance(); };

	virtual void allocateBufferObjectMemory(class VkBufferCreateInfo& createInfo,
		class VertexBuffer& vbo,
		uint32_t memoryFlags = 0,
		bool mappable = false) = 0;
	virtual void destroyBufferObjectMemory(class VertexBuffer& vbo) = 0;

	virtual void mapMemory(IAllocation* allocation, void** ppData) = 0;
	virtual void unmapMemory(IAllocation* allocation) = 0;
};