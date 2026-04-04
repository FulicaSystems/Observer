#pragma once

#include <vulkan/vulkan.h>

struct SemaphoreCreateInfoT
{
    std::optional<VkSemaphoreTypeCreateInfo> type;
};

class Semaphore
{
  public:
    VkSemaphore handle;
};
