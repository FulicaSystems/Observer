#pragma once

#include "memorymanager.hpp"

#ifdef USE_VMA

#include <glad/vulkan.h>

#include <vk_mem_alloc.h>

class VMAHelper
{
public:
	static void createAllocator(ILowRenderer& api, ILogicalDevice& device, VmaAllocator& allocator);
	static void destroyAllocator(VmaAllocator& allocator);

	static void allocateBufferObjectMemory(VmaAllocator& allocator,
		VkBufferCreateInfo& createInfo,
		VertexBuffer& vbo,
		bool mappable = false);
	static void destroyBufferObjectMemory(VmaAllocator& allocator, VertexBuffer& vbo);

	static void mapMemory(VmaAllocator& allocator, VmaAllocation& allocation, void** ppData);
	static void unmapMemory(VmaAllocator& allocator, VmaAllocation& allocation);
};

class Alloc_VMA : public IAllocation
{
public:
	VmaAllocation allocation;
};

class Allocator_VMA : public IMemoryAllocator
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

#endif