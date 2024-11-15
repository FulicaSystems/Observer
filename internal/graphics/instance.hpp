#pragma once

#include <memory>
#include <vector>

#include <vulkan/vulkan.h>

class Context;

class Instance
{
private:
    const Context& cx;

private:
    std::unique_ptr<VkInstance> m_handle;

public:
    Instance(const Context& cx);
    ~Instance();

    inline VkInstance getHandle() const { return *m_handle; }
};