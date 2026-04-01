#pragma once

#include <deque>
#include <memory>

#include <vulkan/vulkan.h>

class AllocationI
{
  public:
    virtual ~AllocationI() = 0;
};

class AllocatorI
{
  public:
    virtual void createAllocatorInstance(VkInstance instance, VkPhysicalDevice pdevice,
                                         VkDevice ldevice) = 0;
    virtual void destroyAllocatorInstance(VkDevice ldevice) = 0;

    // TODO : do not use custom class PhysicalDevice as function argument
    virtual void allocateBufferObjectMemory(VkDevice ldevice, class PhysicalDevice pdevice,
                                            class VkBufferCreateInfo& createInfo,
                                            class IVertexBuffer* vbo, uint32_t memoryFlags = 0,
                                            bool mappable = false) = 0;
    virtual void destroyBufferObjectMemory(VkDevice ldevice, class IVertexBuffer* vbo) = 0;

    virtual void mapMemory(VkDevice ldevice, AllocationI* allocation, void** ppData) = 0;
    virtual void unmapMemory(VkDevice ldevice, AllocationI* allocation) = 0;
};

class AllocatorBackendABC : public AllocatorI
{
};

class MemoryAllocator
{
  private:
    std::unique_ptr<AllocatorBackendABC> m_backend;
};

// TODO : move below to impl folder

// #ifdef USE_VMA

#include <vk_mem_alloc.h>

class VulkanMemoryAllocation : public AllocationI
{
  public:
    VmaAllocation allocation;
};

class VulkanMemoryAllocatorBackend : public AllocatorBackendABC
{
  private:
    VmaAllocator allocator;

  public:
    void createAllocatorInstance(VkInstance instance, VkPhysicalDevice pdevice,
                                 VkDevice ldevice) override;
    void destroyAllocatorInstance(VkDevice ldevice) override;

    void allocateBufferObjectMemory(VkDevice ldevice, class PhysicalDevice pdevice,
                                    VkBufferCreateInfo& createInfo, class IVertexBuffer* vbo,
                                    uint32_t memoryFlags = 0, bool mappable = false) override;
    void destroyBufferObjectMemory(VkDevice ldevice, class IVertexBuffer* vbo) override;

    void mapMemory(VkDevice ldevice, AllocationI* allocation, void** ppData) override;
    void unmapMemory(VkDevice ldevice, AllocationI* allocation) override;
};
// #else
struct MyMemoryAllocator : public AllocatorBackendABC
{
    struct MemoryBlockT
    {
        // memory allocated on the GPU heap
        VkDeviceMemory memory;

        size_t usedSpace = 0;

        bool operator==(MemoryBlockT& other) const
        {
            return memory == other.memory && usedSpace == other.usedSpace;
        }
    };

    // default block size
    size_t blockSize = 1024;
    std::deque<MemoryBlockT> memBlocks;

    MemoryBlockT& findFirstAvailableBlock(VkDevice ldevice, class PhysicalDevice pdevice,
                                          size_t querySize, VkBuffer& buffer,
                                          VkMemoryPropertyFlags memProperties);

    void createAllocatorInstance(VkInstance instance, VkPhysicalDevice pdevice,
                                 VkDevice ldevice) override
    {
    }
    void destroyAllocatorInstance(VkDevice ldevice) override;

    void allocateBufferObjectMemory(VkDevice ldevice, PhysicalDevice pdevice,
                                    VkBufferCreateInfo& createInfo, class IVertexBuffer* vbo,
                                    uint32_t memoryFlags = 0, bool mappable = false) override;
    void destroyBufferObjectMemory(VkDevice ldevice, IVertexBuffer* vbo) override;

    void mapMemory(VkDevice ldevice, AllocationI* allocation, void** ppData) override;
    void unmapMemory(VkDevice ldevice, AllocationI* allocation) override;
};
class MyAllocation : public AllocationI
{
  public:
    // binded memory block
    struct MyMemoryAllocator::MemoryBlock* memoryBlock = nullptr;
    size_t memoryOffset = 0;
};
// #endif