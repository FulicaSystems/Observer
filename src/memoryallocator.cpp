#include "vertexbuffer_vk.hpp"
#include "vmahelper.hpp"

#include "memoryallocator.hpp"

//#ifdef USE_VMA
void VMAMemoryAllocator::createAllocatorInstance(VkInstance instance, VkPhysicalDevice pdevice, VkDevice ldevice)
{
	VMAHelper::createAllocator(instance, pdevice, ldevice, allocator);
}
void VMAMemoryAllocator::destroyAllocatorInstance(VkDevice ldevice)
{
	VMAHelper::destroyAllocator(allocator);
}

void VMAMemoryAllocator::allocateBufferObjectMemory(VkDevice ldevice,
	PhysicalDevice pdevice,
	VkBufferCreateInfo& createInfo,
	class IVertexBuffer* vbo,
	uint32_t memoryFlags,
	bool mappable)
{
	VMAHelper::allocateBufferObjectMemory(allocator, createInfo, vbo, mappable);
}
void VMAMemoryAllocator::destroyBufferObjectMemory(VkDevice ldevice, IVertexBuffer* vbo)
{
	VMAHelper::destroyBufferObjectMemory(allocator, vbo);
}

void VMAMemoryAllocator::mapMemory(VkDevice ldevice, IAllocation* allocation, void** ppData)
{
	VMAHelper::mapMemory(allocator, ((Alloc_VMA*)allocation)->allocation, ppData);
}
void VMAMemoryAllocator::unmapMemory(VkDevice ldevice, IAllocation* allocation)
{
	VMAHelper::unmapMemory(allocator, ((Alloc_VMA*)allocation)->allocation);
}
//#else
// TODO : do not use custom class PhysicalDevice as function argument
MyMemoryAllocator::MemoryBlock& MyMemoryAllocator::findFirstAvailableBlock(VkDevice ldevice,
	PhysicalDevice pdevice,
	size_t querySize,
	VkBuffer& buffer,
	VkMemoryPropertyFlags memProperties)
{
	// using an existing memory block if possible

	for (int i = 0; i < memBlocks.size(); ++i)
	{
		size_t freeSpace = blockSize - memBlocks[i].usedSpace;
		if (freeSpace >= querySize)
			return memBlocks[i];
	}

	VkMemoryRequirements memReq;
	vkGetBufferMemoryRequirements(ldevice, buffer, &memReq);

	// VRAM heap
	VkMemoryAllocateInfo allocInfo = {
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.allocationSize = blockSize,
		.memoryTypeIndex = pdevice.findMemoryType(memReq.memoryTypeBits, memProperties)
	};

	// adding a new memory block

	MemoryBlock newBlock;
	if (vkAllocateMemory(ldevice, &allocInfo, nullptr, &newBlock.memory) != VK_SUCCESS)
		throw std::runtime_error("Failed to allocate vertex buffer memory");

	return memBlocks.emplace_back(newBlock);
}

void MyMemoryAllocator::destroyAllocatorInstance(VkDevice ldevice)
{
	for (int i = 0; i < memBlocks.size(); ++i)
	{
		vkFreeMemory(ldevice, memBlocks[i].memory, nullptr);
	}
	memBlocks.clear();
}

void MyMemoryAllocator::allocateBufferObjectMemory(VkDevice ldevice,
	PhysicalDevice pdevice,
	VkBufferCreateInfo& createInfo,
	class IVertexBuffer* vbo,
	uint32_t memoryFlags,
	bool mappable)
{
	VertexBuffer_Vk* vk = (VertexBuffer_Vk*)vbo;

	if (vkCreateBuffer(ldevice, &createInfo, nullptr, &vk->buffer) != VK_SUCCESS)
		throw std::runtime_error("Failed to create vertex buffer");

	Alloc* alloc = (Alloc*)vk->alloc;

	// binding memory block
	MemoryBlock& block = findFirstAvailableBlock(ldevice, pdevice, vk->bufferSize, vk->buffer, memoryFlags);
	alloc->memoryOffset = block.usedSpace;
	vkBindBufferMemory(ldevice, vk->buffer, block.memory, block.usedSpace);

	// marking space as taken
	block.usedSpace += vk->bufferSize;
	alloc->memoryBlock = &block;
}
void MyMemoryAllocator::destroyBufferObjectMemory(VkDevice ldevice, IVertexBuffer* vbo)
{
	VertexBuffer_Vk* vk = (VertexBuffer_Vk*)vbo;

	MemoryBlock* currentBlock = ((Alloc*)vk->alloc)->memoryBlock;
	currentBlock->usedSpace -= vk->bufferSize;
	if (currentBlock->usedSpace <= 0)
	{
		vkFreeMemory(ldevice, currentBlock->memory, nullptr);
		memBlocks.erase(std::remove(memBlocks.begin(), memBlocks.end(), *currentBlock));
	}
}

void MyMemoryAllocator::mapMemory(VkDevice ldevice, IAllocation* allocation, void** ppData)
{
	Alloc* alloc = (Alloc*)allocation;
	// mapping the whole block
	vkMapMemory(ldevice, alloc->memoryBlock->memory, 0, blockSize, 0, ppData);

	// mapping vertex buffer's offset and size
	//vkMapMemory(device->vkdevice, alloc->memoryBlock->memory, vbo.memoryOffset, vbo.bufferSize, 0, ppData);
}
void MyMemoryAllocator::unmapMemory(VkDevice ldevice, IAllocation* allocation)
{
	Alloc* alloc = (Alloc*)allocation;
	vkUnmapMemory(ldevice, alloc->memoryBlock->memory);
}
//#endif

#ifdef USE_VMA
#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>
#endif