#include <cassert>
#include <iostream>

#include "context.hpp"
#include "device.hpp"
#include "device/memory/buffer.hpp"
#include "device/memory/descriptor.hpp"

#include "pipeline.hpp"

void Pipeline::recreateDescriptorSets(const BufferingTypeE& type)
{
    assert(ci.device);

    ci.type = type;
    int backBufferCount = static_cast<uint32_t>(type);
    const auto& cx = ci.device->getContext();

    m_descriptorBlocks.clear();
    m_descriptorBlocks.reserve(backBufferCount);

    for (int i = 0; i < backBufferCount; ++i)
    {
        VkDescriptorPoolCreateInfo createInfo = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .maxSets = static_cast<uint32_t>(m_setLayouts.size()),
            .poolSizeCount = static_cast<uint32_t>(ci.poolSizes.size()),
            .pPoolSizes = ci.poolSizes.data(),
        };

        m_descriptorBlocks.emplace_back(std::make_unique<DescriptorBlock>());
        auto& block = m_descriptorBlocks[i];
        VkResult res =
            cx->CreateDescriptorPool(ci.device->getHandle(), &createInfo, nullptr, &block->pool);
        if (res != VK_SUCCESS)
            std::cerr << "Failed to create descriptor pool : " << res << std::endl;

        VkDescriptorSetAllocateInfo allocInfo = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = block->pool,
            .descriptorSetCount = static_cast<uint32_t>(m_setLayouts.size()),
            .pSetLayouts = m_setLayouts.data(),
        };

        for (int j = 0; j < ci.setDescriptions.size(); ++j)
        {
            const auto& desc = ci.setDescriptions[j];

            block->sets[desc.frequency].resize(desc.setLayoutBindings.size());
            res = cx->AllocateDescriptorSets(ci.device->getHandle(), &allocInfo,
                                             block->sets[desc.frequency].data());
            if (res != VK_SUCCESS)
                std::cerr << "Failed to allocate descriptor sets : " << res << std::endl;
        }
    }
}

void Pipeline::writeDescriptorSets(const DescriptorFrequencyE frequency, const uint32_t setIndex,
                                   const UniformBuffer& ubo) const
{
    const auto& buff = ubo.getBuffer();
    assert((size_t)buff->size == m_descriptorBlocks.size() * ubo.size);

    for (int i = 0; i < m_descriptorBlocks.size(); ++i)
    {
        const auto& sets = m_descriptorBlocks[i]->sets[frequency];

        VkDescriptorBufferInfo bufferInfo = {
            .buffer = buff->handle,
            .offset = i * ubo.size,
            .range = ubo.size,
        };

        VkWriteDescriptorSet writes = VkWriteDescriptorSet{
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = sets[setIndex],
            .dstBinding = 0,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .pBufferInfo = &bufferInfo,
            .pTexelBufferView = nullptr,
        };
        ci.device->getContext()->UpdateDescriptorSets(ci.device->getHandle(), 1, &writes, 0,
                                                      nullptr);
    }
}
