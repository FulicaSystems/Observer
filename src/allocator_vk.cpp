#include <algorithm>
#include <stdexcept>

#include "vertexbufferdesc_vk.hpp"
#include "graphicsdevice_vk.hpp"

#include "allocator_vk.hpp"

MyAllocator_Vk::MemoryBlock& MyAllocator_Vk::findFirstAvailableBlock(size_t querySize, VkBuffer& buffer, VkMemoryPropertyFlags memProperties)
{
	// using an existing memory block if possible

	for (int i = 0; i < memBlocks.size(); ++i)
	{
		size_t freeSpace = blockSize - memBlocks[i].usedSpace;
		if (freeSpace >= querySize)
			return memBlocks[i];
	}

	const VkDevice& vkdevice = ((LogicalDevice_Vk*)device)->vkdevice;

	VkMemoryRequirements memReq;
	vkGetBufferMemoryRequirements(vkdevice, buffer, &memReq);

	// VRAM heap
	VkMemoryAllocateInfo allocInfo = {
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.allocationSize = blockSize,
		.memoryTypeIndex = ((LogicalDevice_Vk*)device)->pdevice.findMemoryType(memReq.memoryTypeBits, memProperties)
	};

	// adding a new memory block

	MemoryBlock newBlock;
	if (vkAllocateMemory(vkdevice, &allocInfo, nullptr, &newBlock.memory) != VK_SUCCESS)
		throw std::runtime_error("Failed to allocate vertex buffer memory");

	return memBlocks.emplace_back(newBlock);
}

void MyAllocator_Vk::destroyAllocatorInstance()
{
	for (int i = 0; i < memBlocks.size(); ++i)
	{
		vkFreeMemory(((LogicalDevice_Vk*)device)->vkdevice, memBlocks[i].memory, nullptr);
	}
	memBlocks.clear();
}

void MyAllocator_Vk::allocateBufferObjectMemory(VkBufferCreateInfo& createInfo, size_t bufferSize, IVertexBufferLocalDesc* desc, uint32_t memoryFlags, bool mappable)
{
	VertexBufferDesc_Vk* localDesc = (VertexBufferDesc_Vk*)desc;

	if (vkCreateBuffer(((LogicalDevice_Vk*)device)->vkdevice, &createInfo, nullptr, &localDesc->buffer) != VK_SUCCESS)
		throw std::runtime_error("Failed to create vertex buffer");

	MyAlloc_Vk* alloc = (MyAlloc_Vk*)localDesc->alloc;

	// binding memory block
	MemoryBlock& block = findFirstAvailableBlock(bufferSize, localDesc->buffer, memoryFlags);
	alloc->memoryOffset = block.usedSpace;
	vkBindBufferMemory(((LogicalDevice_Vk*)device)->vkdevice, localDesc->buffer, block.memory, block.usedSpace);

	// marking space as taken
	block.usedSpace += bufferSize;
	alloc->memoryBlock = &block;
}

void MyAllocator_Vk::destroyBufferObjectMemory(IVertexBufferLocalDesc* desc, size_t bufferSize)
{
	VertexBufferDesc_Vk* localDesc = (VertexBufferDesc_Vk*)desc;

	MemoryBlock* currentBlock = ((MyAlloc_Vk*)localDesc->alloc)->memoryBlock;
	currentBlock->usedSpace -= bufferSize;
	if (currentBlock->usedSpace <= 0)
	{
		vkFreeMemory(((LogicalDevice_Vk*)device)->vkdevice, currentBlock->memory, nullptr);
		memBlocks.erase(std::remove(memBlocks.begin(), memBlocks.end(), *currentBlock));
	}
}

void MyAllocator_Vk::mapMemory(IAllocation* allocation, void** ppData)
{
	MyAlloc_Vk* alloc = (MyAlloc_Vk*)allocation;
	// mapping the whole block
	vkMapMemory(((LogicalDevice_Vk*)device)->vkdevice, alloc->memoryBlock->memory, 0, blockSize, 0, ppData);

	// mapping vertex buffer's offset and size
	//vkMapMemory(device->vkdevice, alloc->memoryBlock->memory, vbo.memoryOffset, vbo.bufferSize, 0, ppData);
}

void MyAllocator_Vk::unmapMemory(IAllocation* allocation)
{
	MyAlloc_Vk* alloc = (MyAlloc_Vk*)allocation;
	vkUnmapMemory(((LogicalDevice_Vk*)device)->vkdevice, alloc->memoryBlock->memory);
}