#pragma once

#include <glad/vulkan.h>

class CommandBuffer_Vk
{
private:
	VkCommandBuffer commandBuffer;

public:
	void reset();

	void beginRecord(VkCommandBufferUsageFlags flags = 0);
	void endRecord();

	VkCommandBuffer& get();
};