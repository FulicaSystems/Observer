#include <stdexcept>

#include "commandbuffer.hpp"

void CommandBuffer::reset()
{
	vkResetCommandBuffer(commandBuffer, 0);
}

void CommandBuffer::beginRecord(VkCommandBufferUsageFlags flags)
{
	VkCommandBufferBeginInfo commandBufferBeginInfo = {
	.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
	.flags = flags,
	.pInheritanceInfo = nullptr
	};

	if (vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo) != VK_SUCCESS)
		throw std::runtime_error("Failed to begin recording command buffer");
}

void CommandBuffer::endRecord()
{
	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		throw std::runtime_error("Failed to record command buffer");
}

VkCommandBuffer& CommandBuffer::getVkBuffer()
{
	return commandBuffer;
}