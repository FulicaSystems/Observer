#pragma once

#include <memory>

#include <vulkan/vulkan.h>

class Context;

class Instance
{
private:
    std::unique_ptr<VkInstance> m_handle;

public:
    Instance(const Context& cx);
};