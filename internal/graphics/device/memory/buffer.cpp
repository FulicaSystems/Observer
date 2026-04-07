#pragma once

#include <cassert>

#include "device/device.hpp"

#include "buffer.hpp"

UniformBuffer::UniformBuffer(std::shared_ptr<DescriptorCreateInfoT> createInfo)
    : DescriptorABC(createInfo)
{
    auto ci = std::dynamic_pointer_cast<UniformBufferCreateInfoT>(createInfo);
    assert(ci);
    buffer = ci->devicePtr->createBuffer(BufferCreateInfoT{
        .size = ci->size,
        .usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
    });
    ci->devicePtr->mapBufferMemory(buffer, &mappedMemory);
}
