#pragma once

#include <unordered_map>

#include <glad/vulkan.h>

#include "graphicsobject.hpp"
#include "graphicsdevice.hpp"

struct VertexBuffer;

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

	void create() override {}
	void destroy() override;

	MemoryBlock& getAvailableBlock(size_t querySize, VkBuffer& buffer, VkMemoryPropertyFlags memProperties);
};

#include <vk_mem_alloc.h>

class VMAHelper : public IGraphicsObject
{
private:
	LowRenderer& low;
	LogicalDevice& ldevice;

	VmaAllocator allocator;

public:
	VMAHelper(LowRenderer& low, LogicalDevice& ldevice);

	void create() override;
	void destroy() override;

	void allocateBufferObjectMemory(VkBufferCreateInfo& createInfo,
		VertexBuffer& vbo,
		bool mappable = false);

	void destroyBufferObjectMemory(VertexBuffer& vbo);

	void mapMemory(VmaAllocation& allocation, void** ppData);
	void unmapMemory(VmaAllocation& allocation);
};