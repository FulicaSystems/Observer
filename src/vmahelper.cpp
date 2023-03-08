#include "lowrenderer.hpp"
#include "graphicsdevice.hpp"
#include "vertexbuffer.hpp"

#include "vmahelper.hpp"

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

	vmaCreateBuffer(allocator, &createInfo, &allocInfo, &vbo.buffer, &vbo.allocation, nullptr);
}

void VMAHelper::destroyBufferObjectMemory(VmaAllocator& allocator, VertexBuffer& vbo)
{
	//vmaDestroyBuffer(allocator, vbo.buffer, vbo.allocation);
	vmaDestroyBuffer(allocator, VK_NULL_HANDLE, vbo.allocation);
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