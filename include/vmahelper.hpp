#pragma once

#include <glad/vulkan.h>

#include <vk_mem_alloc.h>

class LowRenderer;
class LogicalDevice;
class VertexBuffer;

static class VMAHelper
{
public:
	static void createAllocator(LowRenderer& api, LogicalDevice& device, VmaAllocator& allocator);
	static void destroyAllocator(VmaAllocator& allocator);

	static void allocateBufferObjectMemory(VmaAllocator& allocator,
		VkBufferCreateInfo& createInfo,
		VertexBuffer& vbo,
		bool mappable = false);
	static void destroyBufferObjectMemory(VmaAllocator& allocator, VertexBuffer& vbo);

	static void mapMemory(VmaAllocator& allocator, VmaAllocation& allocation, void** ppData);
	static void unmapMemory(VmaAllocator& allocator, VmaAllocation& allocation);
};