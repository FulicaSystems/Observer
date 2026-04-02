#pragma once

#include <vulkan/vulkan.h>

#include <vk_mem_alloc.h>

struct BufferCreateInfoT
{
    VkDeviceSize size;
    VkBufferUsageFlags usage;
    VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE;
};

class Buffer
{
  public:
    // TODO : make allocation abstraction
    VmaAllocation memory;
    VkBuffer handle;
};