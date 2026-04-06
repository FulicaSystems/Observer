#include <iostream>
#include <cassert>

#include "device.hpp"
#include "context.hpp"

#include "pipeline.hpp"

void Pipeline::recreateDescriptorSets(const BufferingTypeE& type)
{
    assert(ci.device);

    ci.type = type;
    int backBufferCount = static_cast<uint32_t>(type);
    const auto& cx = ci.device->getContext();

    m_descriptorBlock.reset();
    m_descriptorBlock = std::make_unique<DescriptorBlock>();

    VkDescriptorPoolCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .maxSets = backBufferCount * static_cast<uint32_t>(m_setLayouts.size()),
        .poolSizeCount = static_cast<uint32_t>(ci.poolSizes.size()),
        .pPoolSizes = ci.poolSizes.data(),
    };

    VkResult res =
        cx->CreateDescriptorPool(ci.device->getHandle(), &createInfo, nullptr, &m_descriptorBlock->pool);
    if (res != VK_SUCCESS)
        std::cerr << "Failed to create descriptor pool : " << res << std::endl;

    m_descriptorBlock->sets.resize(backBufferCount);
    for (int i = 0; i < backBufferCount; ++i)
    {
        VkDescriptorSetAllocateInfo allocInfo = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = m_descriptorBlock->pool,
            .descriptorSetCount = static_cast<uint32_t>(m_setLayouts.size()),
            .pSetLayouts = m_setLayouts.data(),
        };

        m_descriptorBlock->sets[i].resize(m_setLayouts.size());
        res = cx->AllocateDescriptorSets(ci.device->getHandle(), &allocInfo, m_descriptorBlock->sets[i].data());
        if (res != VK_SUCCESS)
            std::cerr << "Failed to allocate descriptor sets : " << res << std::endl;
    }
}
