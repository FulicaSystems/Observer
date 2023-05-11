#pragma once

#include <glad/vulkan.h>

#include <vk_mem_alloc.h>

#include "memorymanager.hpp"

class VMAHelper
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

class VMAHelperAlloc : public IAllocation
{
public:
	VmaAllocation allocation;
};

class VMAHelperAllocator : public IMemoryAllocator
{
private:
	VmaAllocator allocator;

	void createAllocatorInstance() override;
	void destroyAllocatorInstance() override;

public:
	void allocateBufferObjectMemory(VkBufferCreateInfo& createInfo,
		VertexBuffer& vbo,
		uint32_t memoryFlags = 0,
		bool mappable = false) override;
	void destroyBufferObjectMemory(VertexBuffer& vbo) override;

	void mapMemory(IAllocation* allocation, void** ppData) override;
	void unmapMemory(IAllocation* allocation) override;
};