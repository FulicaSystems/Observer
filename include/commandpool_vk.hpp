#pragma once

#include <deque>

#include <glad/vulkan.h>

#include "utils/derived.hpp"

#include "commandpool.hpp"
#include "commandbuffer_vk.hpp"

class CommandPool_Vk : public ICommandPool
{
	SUPER(ICommandPool)

private:
	VkCommandPool commandPool;
	std::deque<CommandBuffer_Vk> cbos;

	void vulkanCommandPool();

public:
	void create(class ILowRenderer* api, ILogicalDevice* device) override;
	void destroy() override;

	CommandBuffer_Vk createFloatingCommandBuffer();
	CommandBuffer_Vk& createCommandBuffer();

	void destroyFloatingCommandBuffer(CommandBuffer_Vk& cbo);
	void destroyCommandBuffer(const int index);

	CommandBuffer_Vk& getCmdBufferByIndex(const int index);
};