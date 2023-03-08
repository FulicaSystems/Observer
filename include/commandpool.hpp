#pragma once

#include <unordered_map>

#include <glad/vulkan.h>

#include "graphicsobject.hpp"

#include "commandbuffer.hpp"

class LowRenderer;

class CommandPool : public IDerived<CommandPool, IGraphicsObject>
{
private:
	VkCommandPool commandPool;
	std::unordered_map<int, CommandBuffer> cbos;

	void vulkanCommandPool();

public:
	void create(LowRenderer* api, LogicalDevice* device) override;
	void destroy() override;

	CommandBuffer createFloatingCommandBuffer();
	CommandBuffer& createCommandBuffer();

	void destroyFloatingCommandBuffer(CommandBuffer& cbo);
	void destroyCommandBuffer(const int index);

	CommandBuffer& getCmdBufferByIndex(const int index);
};