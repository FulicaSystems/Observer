#pragma once

#include <unordered_map>

#include <glad/vulkan.h>

#include "graphicsobject.hpp"
#include "graphicsdevice.hpp"

struct MemoryBlock
{
	// memory allocated on the GPU heap
	VkDeviceMemory memory;

	size_t usedSpace = 0;
};

// TODO : improve custom allocator
// temporary custom solution
class MemoryAllocator : public IGraphicsObject
{
private:
	// logical device used by this memory allocator
	LogicalDevice& ldevice;

	// default block size
	size_t blockSize = 1024;

	std::unordered_map<int, MemoryBlock> memBlocks;

public:
	MemoryAllocator(LogicalDevice& ldevice);

	void create() {}
	void destroy();

	MemoryBlock& getAvailableBlock(size_t querySize, VkBuffer& buffer, VkMemoryPropertyFlags memProperties);
};

//#include <vk_mem_alloc.h>

class VulkanMemoryAllocator
{

};
