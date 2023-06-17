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
		class VertexBufferDesc_Vk* desc,
		bool mappable = false);
	static void destroyBufferObjectMemory(VmaAllocator& allocator, class VertexBufferDesc_Vk* desc);

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
		size_t bufferSize,
		class IVertexBufferLocalDesc* desc,
		uint32_t memoryFlags = 0,
		bool mappable = false) override;
	void destroyBufferObjectMemory(class IVertexBufferLocalDesc* desc, size_t bufferSize) override;

	void mapMemory(IAllocation* allocation, void** ppData) override;
	void unmapMemory(IAllocation* allocation) override;
};

#endif