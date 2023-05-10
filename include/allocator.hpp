#pragma once

#include <deque>

#include <glad/vulkan.h>

#include "memorymanager.hpp"

// TODO : improve custom allocator
class MyAllocator : public IMemoryAllocator
{
public:
	struct MemoryBlock
	{
		// memory allocated on the GPU heap
		VkDeviceMemory memory;

		size_t usedSpace = 0;

		bool operator==(MemoryBlock& other) const
		{
			return memory == other.memory && usedSpace == other.usedSpace;
		}
	};

private:
	// default block size
	size_t blockSize = 1024;
	std::deque<MemoryBlock> memBlocks;

	MemoryBlock& findFirstAvailableBlock(size_t querySize, VkBuffer& buffer, VkMemoryPropertyFlags memProperties);

protected:
	void createAllocatorInstance() override {}
	void destroyAllocatorInstance() override;

public:
	void allocateBufferObjectMemory(VkBufferCreateInfo& createInfo,
		class VertexBuffer& vbo,
		uint32_t memoryFlags = 0,
		bool mappable = false) override;
	void destroyBufferObjectMemory(class VertexBuffer& vbo) override;

	void mapMemory(IAllocation* allocation, void** ppData) override;
	void unmapMemory(IAllocation* allocation) override;
};

class MyAlloc : public IAllocation
{
public:
	// binded memory block
	struct MyAllocator::MemoryBlock* memoryBlock = nullptr;
	size_t memoryOffset = 0;
};