#pragma once

#include <deque>

#include <vk_mem_alloc.h>
#include <glad/vulkan.h>

class IAllocation
{
public:
	virtual ~IAllocation() {}
};
struct IMemoryAllocator
{
	virtual void createAllocatorInstance(VkInstance instance, VkPhysicalDevice pdevice, VkDevice ldevice) = 0;
	virtual void destroyAllocatorInstance(VkDevice ldevice) = 0;

	//IMemoryAllocator() { createAllocatorInstance(nullptr, nullptr, nullptr); }
	//virtual ~IMemoryAllocator() { destroyAllocatorInstance(nullptr); }

	// TODO : do not use custom class PhysicalDevice as function argument
	virtual void allocateBufferObjectMemory(VkDevice ldevice,
		class PhysicalDevice pdevice,
		class VkBufferCreateInfo& createInfo,
		class IVertexBuffer* vbo,
		uint32_t memoryFlags = 0,
		bool mappable = false) = 0;
	virtual void destroyBufferObjectMemory(VkDevice ldevice,
		class IVertexBuffer* vbo) = 0;

	virtual void mapMemory(VkDevice ldevice, IAllocation* allocation, void** ppData) = 0;
	virtual void unmapMemory(VkDevice ldevice, IAllocation* allocation) = 0;
};


//#ifdef USE_VMA
struct VMAMemoryAllocator : public IMemoryAllocator
{
	VmaAllocator allocator;

	void createAllocatorInstance(VkInstance instance, VkPhysicalDevice pdevice, VkDevice ldevice) override;
	void destroyAllocatorInstance(VkDevice ldevice) override;

	void allocateBufferObjectMemory(VkDevice ldevice,
		class PhysicalDevice pdevice,
		VkBufferCreateInfo& createInfo,
		class IVertexBuffer* vbo,
		uint32_t memoryFlags = 0,
		bool mappable = false) override;
	void destroyBufferObjectMemory(VkDevice ldevice, class IVertexBuffer* vbo) override;

	void mapMemory(VkDevice ldevice, IAllocation* allocation, void** ppData) override;
	void unmapMemory(VkDevice ldevice, IAllocation* allocation) override;
};
class Alloc_VMA : public IAllocation
{
public:
	VmaAllocation allocation;
};
//#else
struct MyMemoryAllocator : public IMemoryAllocator
{
	struct MemoryBlock
	{
		// memory allocated on the GPU heap
		VkDeviceMemory memory;

		size_t usedSpace = 0;

		bool operator==(MemoryBlock& other) const
		{
			return memory == other.memory && usedSpace == other.usedSpace;
		}
	};

	// default block size
	size_t blockSize = 1024;
	std::deque<MemoryBlock> memBlocks;

	MemoryBlock& findFirstAvailableBlock(VkDevice ldevice,
		class PhysicalDevice pdevice,
		size_t querySize,
		VkBuffer& buffer,
		VkMemoryPropertyFlags memProperties);

	void createAllocatorInstance(VkInstance instance, VkPhysicalDevice pdevice, VkDevice ldevice) override {}
	void destroyAllocatorInstance(VkDevice ldevice) override;

	void allocateBufferObjectMemory(VkDevice ldevice,
		PhysicalDevice pdevice,
		VkBufferCreateInfo& createInfo,
		class IVertexBuffer* vbo,
		uint32_t memoryFlags = 0,
		bool mappable = false) override;
	void destroyBufferObjectMemory(VkDevice ldevice, IVertexBuffer* vbo) override;

	void mapMemory(VkDevice ldevice, IAllocation* allocation, void** ppData) override;
	void unmapMemory(VkDevice ldevice, IAllocation* allocation) override;
};
class Alloc : public IAllocation
{
public:
	// binded memory block
	struct MyMemoryAllocator::MemoryBlock* memoryBlock = nullptr;
	size_t memoryOffset = 0;
};
//#endif