#pragma once

#include <deque>

#include <glad/vulkan.h>

#include "graphicsobject.hpp"

struct MemoryBlock
{
	// memory allocated on the GPU heap
	VkDeviceMemory memory;

	size_t usedSpace = 0;
};

// TODO : improve custom allocator
class MemoryAllocator : public IDerived<MemoryAllocator, IGraphicsObject>
{
private:
	// default block size
	size_t blockSize = 1024;

	std::deque<MemoryBlock> memBlocks;

public:
	void destroy() override;

	MemoryBlock& getAvailableBlock(size_t querySize, VkBuffer& buffer, VkMemoryPropertyFlags memProperties);
};