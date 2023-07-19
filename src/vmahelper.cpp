#include "lowrenderer_vk.hpp"

#include "vmahelper.hpp"

#ifdef USE_VMA

#include "vertexbuffer_vk.hpp"

void VMAHelper::createAllocator(VkInstance instance, VkPhysicalDevice pdevice, VkDevice device, VmaAllocator& allocator)
{
	VmaAllocatorCreateInfo createInfo = {
		.physicalDevice = pdevice,
		.device = device,
		.instance = instance,
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
	IVertexBuffer* vbo,
	bool mappable)
{
	VertexBuffer_Vk* vk = (VertexBuffer_Vk*)vbo;

	VmaAllocationCreateInfo allocInfo = {
		.flags = mappable ? VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT : (uint32_t)0,
		.usage = VMA_MEMORY_USAGE_AUTO,
	};

	vmaCreateBuffer(allocator, &createInfo, &allocInfo, &vk->buffer, &((Alloc_VMA*)vk->alloc)->allocation, nullptr);
}

void VMAHelper::destroyBufferObjectMemory(VmaAllocator& allocator, IVertexBuffer* vbo)
{
	VertexBuffer_Vk* vk = (VertexBuffer_Vk*)vbo;
	//vmaDestroyBuffer(allocator, vbo.buffer, ((Alloc_VMA*)vk->alloc)->allocation);
	vmaDestroyBuffer(allocator, VK_NULL_HANDLE, ((Alloc_VMA*)vk->alloc)->allocation);
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

#endif