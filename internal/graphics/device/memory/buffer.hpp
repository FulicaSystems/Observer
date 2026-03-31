#pragma once

#include <vulkan/vulkan.h>

struct BufferCreateInfoT
{
};

class Buffer
{
  public:
    VkBuffer handle;

    // allocation
    //	- custom
    //	- vma
};