#include "vertexbuffer.hpp"

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

	const VkDevice& device = ldevice.getVkLDevice();

	VkMemoryRequirements memReq;
	vkGetBufferMemoryRequirements(device, buffer, &memReq);

	// VRAM heap
	VkMemoryAllocateInfo allocInfo = {
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.allocationSize = blockSize,
		.memoryTypeIndex = ldevice.getPDevice().findMemoryType(memReq.memoryTypeBits, memProperties)
	};

	// adding a new memory block

	MemoryBlock newBlock;
	if (vkAllocateMemory(device, &allocInfo, nullptr, &newBlock.memory) != VK_SUCCESS)
		throw std::exception("Failed to allocate vertex buffer memory");

	int index = memBlocks.size();
	memBlocks[index] = newBlock;
	return memBlocks[index];
}

MemoryAllocator::MemoryAllocator(LogicalDevice& ldevice)
	: ldevice(ldevice)
{
}

void MemoryAllocator::destroy()
{
	for (int i = 0; i < memBlocks.size(); ++i)
	{
		vkFreeMemory(ldevice.getVkLDevice(), memBlocks[i].memory, nullptr);
	}
	memBlocks.clear();
}

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

VMAHelper::VMAHelper(LowRenderer& low, LogicalDevice& ldevice)
	: low(low), ldevice(ldevice)
{
}

void VMAHelper::create()
{
	VmaAllocatorCreateInfo createInfo = {
		.physicalDevice = ldevice.getVkPDevice(),
		.device = ldevice.getVkLDevice(),
		.instance = low.instance,
		.vulkanApiVersion = VK_API_VERSION_1_3
	};

	vmaCreateAllocator(&createInfo, &allocator);
}

void VMAHelper::destroy()
{
	vmaDestroyAllocator(allocator);
}

void VMAHelper::allocateBufferObjectMemory(VkBufferCreateInfo& createInfo,
	VertexBuffer& vbo,
	bool mappable)
{
	VmaAllocationCreateInfo allocInfo = {
		.flags = mappable ? VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT : (uint32_t)0,
		.usage = VMA_MEMORY_USAGE_AUTO,
	};

	vmaCreateBuffer(allocator, &createInfo, &allocInfo, &vbo.buffer, &vbo.allocation, nullptr);
}

void VMAHelper::destroyBufferObjectMemory(VertexBuffer& vbo)
{
	//vmaDestroyBuffer(allocator, vbo.buffer, vbo.allocation);
	vmaDestroyBuffer(allocator, VK_NULL_HANDLE, vbo.allocation);
}

void VMAHelper::mapMemory(VmaAllocation& allocation, void** ppData)
{
	vmaMapMemory(allocator, allocation, ppData);
}

void VMAHelper::unmapMemory(VmaAllocation& allocation)
{
	vmaUnmapMemory(allocator, allocation);
}