#include <algorithm>
#include <stdexcept>

#include "vertexbuffer.hpp"
#include "graphicsdevice.hpp"

#include "allocator.hpp"

MyAllocator::MemoryBlock& MyAllocator::findFirstAvailableBlock(size_t querySize, VkBuffer& buffer, VkMemoryPropertyFlags memProperties)
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
		throw std::runtime_error("Failed to allocate vertex buffer memory");

	return memBlocks.emplace_back(newBlock);
}

void MyAllocator::destroyAllocatorInstance()
{
	for (int i = 0; i < memBlocks.size(); ++i)
	{
		vkFreeMemory(device->vkdevice, memBlocks[i].memory, nullptr);
	}
	memBlocks.clear();
}

void MyAllocator::allocateBufferObjectMemory(VkBufferCreateInfo& createInfo, VertexBuffer& vbo, uint32_t memoryFlags, bool mappable)
{
	if (vkCreateBuffer(device->vkdevice, &createInfo, nullptr, &vbo.buffer) != VK_SUCCESS)
		throw std::runtime_error("Failed to create vertex buffer");

	MyAlloc* alloc = (MyAlloc*)vbo.alloc;

	// binding memory block
	MemoryBlock& block = findFirstAvailableBlock(vbo.bufferSize, vbo.buffer, memoryFlags);
	alloc->memoryOffset = block.usedSpace;
	vkBindBufferMemory(device->vkdevice, vbo.buffer, block.memory, block.usedSpace);

	// marking space as taken
	block.usedSpace += vbo.bufferSize;
	alloc->memoryBlock = &block;
}

void MyAllocator::destroyBufferObjectMemory(VertexBuffer& vbo)
{
	MemoryBlock* currentBlock = ((MyAlloc*)vbo.alloc)->memoryBlock;
	currentBlock->usedSpace -= vbo.bufferSize;
	if (currentBlock->usedSpace <= 0)
	{
		vkFreeMemory(device->vkdevice, currentBlock->memory, nullptr);
		memBlocks.erase(std::remove(memBlocks.begin(), memBlocks.end(), *currentBlock));
	}
}

void MyAllocator::mapMemory(IAllocation* allocation, void** ppData)
{
	MyAlloc* alloc = (MyAlloc*)allocation;
	// mapping the whole block
	vkMapMemory(device->vkdevice, alloc->memoryBlock->memory, 0, blockSize, 0, ppData);

	// mapping vertex buffer's offset and size
	//vkMapMemory(device->vkdevice, alloc->memoryBlock->memory, vbo.memoryOffset, vbo.bufferSize, 0, ppData);
}

void MyAllocator::unmapMemory(IAllocation* allocation)
{
	MyAlloc* alloc = (MyAlloc*)allocation;
	vkUnmapMemory(device->vkdevice, alloc->memoryBlock->memory);
}