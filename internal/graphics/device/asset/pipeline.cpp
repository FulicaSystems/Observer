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

    m_descriptorBlock.reset();
    m_descriptorBlock = std::make_unique<DescriptorBlock>();

    VkDescriptorPoolCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .maxSets = backBufferCount * static_cast<uint32_t>(m_setLayouts.size()),
        .poolSizeCount = static_cast<uint32_t>(ci.poolSizes.size()),
        .pPoolSizes = ci.poolSizes.data(),
    };

    VkResult res = cx->CreateDescriptorPool(ci.device->getHandle(), &createInfo, nullptr,
                                            &m_descriptorBlock->pool);
    if (res != VK_SUCCESS)
        std::cerr << "Failed to create descriptor pool : " << res << std::endl;

    m_descriptorBlock->sets.resize(backBufferCount);
    m_descriptorBlock->descriptors.resize(backBufferCount);
    for (int i = 0; i < backBufferCount; ++i)
    {
        VkDescriptorSetAllocateInfo allocInfo = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = m_descriptorBlock->pool,
            .descriptorSetCount = static_cast<uint32_t>(m_setLayouts.size()),
            .pSetLayouts = m_setLayouts.data(),
        };

        m_descriptorBlock->sets[i].resize(m_setLayouts.size());
        res = cx->AllocateDescriptorSets(ci.device->getHandle(), &allocInfo,
                                         m_descriptorBlock->sets[i].data());
        if (res != VK_SUCCESS)
            std::cerr << "Failed to allocate descriptor sets : " << res << std::endl;

        std::vector<VkWriteDescriptorSet> writes;
        writes.reserve(ci.descriptorCreateInfos.size());
        m_descriptorBlock->descriptors[i].reserve(ci.descriptorCreateInfos.size());
        for (int j = 0; j < ci.descriptorCreateInfos.size(); ++j)
        {
            auto& dci = ci.descriptorCreateInfos[j];
            switch (dci->type)
            {
            case DescriptorTypeE::UNIFORM_BUFFER: {
                m_descriptorBlock->descriptors[i].push_back(std::make_unique<UniformBuffer>(dci));
                const auto ub =
                    static_cast<UniformBuffer*>(m_descriptorBlock->descriptors[i].back().get());

                VkDescriptorBufferInfo bufferInfo = {
                    .buffer = ub->getBuffer()->handle,
                    .offset = 0,
                    .range = ub->getBuffer()->size,
                };

                writes.emplace_back(VkWriteDescriptorSet{
                    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                    .dstSet = m_descriptorBlock->sets[i][dci->setLayoutIndex],
                    .dstBinding = 0,
                    .dstArrayElement = 0,
                    .descriptorCount = 1,
                    .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                    .pBufferInfo = &bufferInfo,
                    .pTexelBufferView = nullptr,
                });
                break;
            }
                // TODO : other descriptor types
            default:
                throw;
                break;
            }
        }
        cx->UpdateDescriptorSets(ci.device->getHandle(), static_cast<uint32_t>(writes.size()),
                                 writes.data(), 0, nullptr);
    }
}
