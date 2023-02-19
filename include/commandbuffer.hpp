#pragma once

#include <glad/vulkan.h>

class CommandBuffer
{
private:
	VkCommandBuffer commandBuffer;

public:
	void reset();

	void beginRecord();
	void endRecord();

	VkCommandBuffer& getBuffer();
};