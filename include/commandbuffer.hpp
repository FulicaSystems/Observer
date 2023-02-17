#pragma once

#include <glad/vulkan.h>

#include "graphicsobject.hpp"

class CommandBuffer : public IGraphicsObject
{
private:
	VkCommandBuffer commandBuffer;

	void vulkanCommandBuffer();

public:
	void create() override;
	void destroy() override;
};