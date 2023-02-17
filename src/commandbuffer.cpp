#include "commandbuffer.hpp"

void CommandBuffer::vulkanCommandBuffer()
{
	VkDevice ldevice = device.getVkLDevice();

	VkCommandBufferAllocateInfo allocInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.commandPool = commandPool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = 1
	};

	if (vkAllocateCommandBuffers(ldevice, &allocInfo, &commandBuffer) != VK_SUCCESS)
		throw std::exception("Failed to allocate command buffers");
}

void CommandBuffer::create()
{
}

void CommandBuffer::destroy()
{
}