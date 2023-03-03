#pragma once

#include <unordered_map>

#include <glad/vulkan.h>

#include "graphicsobject.hpp"
#include "commandbuffer.hpp"
#include "graphicsdevice.hpp"

class CommandPool : public IGraphicsObject
{
private:
	// logical device used by this command pool
	LogicalDevice& device;

	VkCommandPool commandPool;
	std::unordered_map<int, CommandBuffer> cbos;

	void vulkanCommandPool();

public:
	CommandPool(LogicalDevice& device);

	void create() override;
	void destroy() override;

	void destroyCommandBuffer(const int index);
	void destroyCommandBuffer(CommandBuffer& cbo);

	CommandBuffer createFloatingCommandBuffer();
	CommandBuffer& createCommandBuffer();

	CommandBuffer& getCmdBufferByIndex(const int index);
};