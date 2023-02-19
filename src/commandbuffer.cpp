#include <exception>

#include "commandbuffer.hpp"

void CommandBuffer::reset()
{
	vkResetCommandBuffer(commandBuffer, 0);
}

void CommandBuffer::beginRecord()
{
	VkCommandBufferBeginInfo commandBufferBeginInfo = {
	.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
	.flags = 0,
	.pInheritanceInfo = nullptr
	};

	if (vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo) != VK_SUCCESS)
		throw std::exception("Failed to begin recording command buffer");
}

void CommandBuffer::endRecord()
{
	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		throw std::exception("Failed to record command buffer");
}

VkCommandBuffer& CommandBuffer::getBuffer()
{
	return commandBuffer;
}