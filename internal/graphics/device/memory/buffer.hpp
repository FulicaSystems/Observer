#pragma once

#include <vulkan/vulkan.h>

#include <vk_mem_alloc.h>

#include "descriptor.hpp"

struct BufferCreateInfoT
{
    VkDeviceSize size;
    VkBufferUsageFlags usage;
    VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE;
};

class Buffer
{
  public:
    VkDeviceSize size;
    // TODO : make allocation abstraction
    VmaAllocation memory;
    VkBuffer handle;
};

struct UniformBufferCreateInfoT : public DescriptorCreateInfoT
{
    size_t size;
};

class UniformBuffer : public DescriptorABC
{
  private:
    std::shared_ptr<Buffer> buffer;
    void* mappedMemory;

  public:
    UniformBuffer() = delete;
    UniformBuffer(std::shared_ptr<DescriptorCreateInfoT> createInfo);

  public:
    [[nodiscard]] const std::shared_ptr<Buffer>& getBuffer() const { return buffer; }
};
