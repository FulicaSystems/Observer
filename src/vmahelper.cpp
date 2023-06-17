#include "vmahelper.hpp"

#ifdef USE_VMA

#include "lowrenderer_vk.hpp"
#include "graphicsdevice_vk.hpp"
#include "vertexbuffer.hpp"

void VMAHelper::createAllocator(ILowRenderer& api, ILogicalDevice& device, VmaAllocator& allocator)
{
	VmaAllocatorCreateInfo createInfo = {
		.physicalDevice = ((LogicalDevice_Vk&)device).pdevice.vkpdevice,
		.device = ((LogicalDevice_Vk&)device).vkdevice,
		.instance = ((LowRenderer_Vk&)api).instance,
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
	VertexBufferDesc_Vk* desc,
	bool mappable)
{
	VmaAllocationCreateInfo allocInfo = {
		.flags = mappable ? VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT : (uint32_t)0,
		.usage = VMA_MEMORY_USAGE_AUTO,
	};

	vmaCreateBuffer(allocator, &createInfo, &allocInfo, &desc->buffer, &((Alloc_VMA*)desc->alloc)->allocation, nullptr);
}

void VMAHelper::destroyBufferObjectMemory(VmaAllocator& allocator, VertexBufferDesc_Vk* desc)
{
	//vmaDestroyBuffer(allocator, vbo.buffer, ((Alloc_VMA*)desc->alloc)->allocation);
	vmaDestroyBuffer(allocator, VK_NULL_HANDLE, ((Alloc_VMA*)desc->alloc)->allocation);
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

void Allocator_VMA::createAllocatorInstance()
{
	VMAHelper::createAllocator(*api, *device, allocator);
}

void Allocator_VMA::destroyAllocatorInstance()
{
	VMAHelper::destroyAllocator(allocator);
}

void Allocator_VMA::allocateBufferObjectMemory(VkBufferCreateInfo& createInfo, size_t bufferSize, IVertexBufferLocalDesc* desc, uint32_t memoryFlags, bool mappable)
{
	VMAHelper::allocateBufferObjectMemory(allocator, createInfo, (VertexBufferDesc_Vk*)desc, mappable);
}

void Allocator_VMA::destroyBufferObjectMemory(IVertexBufferLocalDesc* desc, size_t bufferSize)
{
	VMAHelper::destroyBufferObjectMemory(allocator, (VertexBufferDesc_Vk*)desc);
}

void Allocator_VMA::mapMemory(IAllocation* allocation, void** ppData)
{
	VMAHelper::mapMemory(allocator, ((Alloc_VMA*)allocation)->allocation, ppData);
}

void Allocator_VMA::unmapMemory(IAllocation* allocation)
{
	VMAHelper::unmapMemory(allocator, ((Alloc_VMA*)allocation)->allocation);
}

#endif