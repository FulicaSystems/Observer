#include "vertexbuffer.hpp"
#include "graphicsdevice.hpp"

#include "memoryallocator.hpp"

MemoryBlock& MemoryAllocator::getAvailableBlock(size_t querySize, VkBuffer& buffer, VkMemoryPropertyFlags memProperties)
{
	// using an existing memory block if possible

	for (int i = 0; i < memBlocks.size(); ++i)
	{
		size_t freeSpace = blockSize - memBlocks[i].usedSpace;
		if (freeSpace >= querySize)
			return memBlocks[i];
	}

	const VkDevice& vkdevice = device->vkdevice;

	VkMemoryRequirements memReq;
	vkGetBufferMemoryRequirements(vkdevice, buffer, &memReq);

	// VRAM heap
	VkMemoryAllocateInfo allocInfo = {
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.allocationSize = blockSize,
		.memoryTypeIndex = device->pdevice.findMemoryType(memReq.memoryTypeBits, memProperties)
	};

	// adding a new memory block

	MemoryBlock newBlock;
	if (vkAllocateMemory(vkdevice, &allocInfo, nullptr, &newBlock.memory) != VK_SUCCESS)
		throw std::exception("Failed to allocate vertex buffer memory");

	return memBlocks.emplace_back(newBlock);
}

void MemoryAllocator::destroy()
{
	for (int i = 0; i < memBlocks.size(); ++i)
	{
		vkFreeMemory(device->vkdevice, memBlocks[i].memory, nullptr);
	}
	memBlocks.clear();
}