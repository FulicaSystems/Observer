#pragma once

#include <cassert>

#include "device/device.hpp"

#include "buffer.hpp"

UniformBuffer::UniformBuffer(std::shared_ptr<DescriptorCreateInfoT> createInfo)
    : DescriptorABC(createInfo)
{
    auto ci = std::dynamic_pointer_cast<UniformBufferCreateInfoT>(createInfo);
    assert(ci);
    size = ci->size;
    buffer = ci->devicePtr->createBuffer(BufferCreateInfoT{
        .size = ci->backBufferCount * ci->size,
        .usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        .memoryPropertyFlags =
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    });
    ci->devicePtr->mapBufferMemory(buffer, &mappedMemory);
}
