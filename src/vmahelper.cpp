#include "vmahelper.hpp"

#ifdef USE_VMA

#include "lowrenderer.hpp"
#include "graphicsdevice.hpp"
#include "vertexbuffer.hpp"

void VMAHelper::createAllocator(LowRenderer& api, LogicalDevice& device, VmaAllocator& allocator)
{
	VmaAllocatorCreateInfo createInfo = {
		.physicalDevice = device.pdevice.vkpdevice,
		.device = device.vkdevice,
		.instance = api.instance,
		.vulkanApiVersion = VK_API_VERSION_1_3
	};

	vmaCreateAllocator(&createInfo, &allocator);
}

void VMAHelper::destroyAllocator(VmaAllocator& allocator)
{
	vmaDestroyAllocator(allocator);
}

void VMAHelper::allocateBufferObjectMemory(VmaAllocator& allocator,
	VkBufferCreateInfo& createInfo,
	VertexBuffer& vbo,
	bool mappable)
{
	VmaAllocationCreateInfo allocInfo = {
		.flags = mappable ? VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT : (uint32_t)0,
		.usage = VMA_MEMORY_USAGE_AUTO,
	};

	vmaCreateBuffer(allocator, &createInfo, &allocInfo, &vbo.buffer, &((VMAHelperAlloc*)vbo.alloc)->allocation, nullptr);
}

void VMAHelper::destroyBufferObjectMemory(VmaAllocator& allocator, VertexBuffer& vbo)
{
	//vmaDestroyBuffer(allocator, vbo.buffer, ((VMAHelperAlloc*)vbo.alloc)->allocation);
	vmaDestroyBuffer(allocator, VK_NULL_HANDLE, ((VMAHelperAlloc*)vbo.alloc)->allocation);
}

void VMAHelper::mapMemory(VmaAllocator& allocator, VmaAllocation& allocation, void** ppData)
{
	vmaMapMemory(allocator, allocation, ppData);
}

void VMAHelper::unmapMemory(VmaAllocator& allocator, VmaAllocation& allocation)
{
	vmaUnmapMemory(allocator, allocation);
}

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

void VMAHelperAllocator::createAllocatorInstance()
{
	VMAHelper::createAllocator(*api, *device, allocator);
}

void VMAHelperAllocator::destroyAllocatorInstance()
{
	VMAHelper::destroyAllocator(allocator);
}

void VMAHelperAllocator::allocateBufferObjectMemory(VkBufferCreateInfo& createInfo, VertexBuffer& vbo, uint32_t memoryFlags, bool mappable)
{
	VMAHelper::allocateBufferObjectMemory(allocator, createInfo, vbo, mappable);
}

void VMAHelperAllocator::destroyBufferObjectMemory(VertexBuffer& vbo)
{
	VMAHelper::destroyBufferObjectMemory(allocator, vbo);
}

void VMAHelperAllocator::mapMemory(IAllocation* allocation, void** ppData)
{
	VMAHelper::mapMemory(allocator, ((VMAHelperAlloc*)allocation)->allocation, ppData);
}

void VMAHelperAllocator::unmapMemory(IAllocation* allocation)
{
	VMAHelper::unmapMemory(allocator, ((VMAHelperAlloc*)allocation)->allocation);
}

#endif