#pragma once

#include <glad/vulkan.h>
#include <vk_mem_alloc.h>

class VMAHelper
{
public:
	static void createAllocator(VkInstance instance, VkPhysicalDevice pdevice, VkDevice device, VmaAllocator& allocator);
	static void destroyAllocator(VmaAllocator& allocator);

	static void allocateBufferObjectMemory(VmaAllocator& allocator,
		VkBufferCreateInfo& createInfo,
		class IVertexBuffer* vbo,
		bool mappable = false);
	static void destroyBufferObjectMemory(VmaAllocator& allocator, class IVertexBuffer* vbo);

	static void mapMemory(VmaAllocator& allocator, VmaAllocation& allocation, void** ppData);
	static void unmapMemory(VmaAllocator& allocator, VmaAllocation& allocation);
};