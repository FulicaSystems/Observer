#pragma once

#include "graphicsobject.hpp"

class IAllocation
{
};

class IMemoryAllocator : public IDerived<IMemoryAllocator, IGraphicsObject>
{
protected:
	virtual void createAllocatorInstance() = 0;
	virtual void destroyAllocatorInstance() = 0;

public:
	void create(LowRenderer* api, LogicalDevice* device) override { Super::create(api, device); createAllocatorInstance(); };
	void destroy() override { destroyAllocatorInstance(); };

	virtual void allocateBufferObjectMemory(VkBufferCreateInfo& createInfo,
		class VertexBuffer& vbo,
		uint32_t memoryFlags = 0,
		bool mappable = false) = 0;
	virtual void destroyBufferObjectMemory(class VertexBuffer& vbo) = 0;

	virtual void mapMemory(IAllocation* allocation, void** ppData) = 0;
	virtual void unmapMemory(IAllocation* allocation) = 0;
};