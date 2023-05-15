#include <stdexcept>

#include "commandbuffer_vk.hpp"

void CommandBuffer_Vk::reset()
{
	vkResetCommandBuffer(commandBuffer, 0);
}

void CommandBuffer_Vk::beginRecord(VkCommandBufferUsageFlags flags)
{
	VkCommandBufferBeginInfo commandBufferBeginInfo = {
	.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
	.flags = flags,
	.pInheritanceInfo = nullptr
	};

	if (vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo) != VK_SUCCESS)
		throw std::runtime_error("Failed to begin recording command buffer");
}

void CommandBuffer_Vk::endRecord()
{
	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		throw std::runtime_error("Failed to record command buffer");
}

VkCommandBuffer& CommandBuffer_Vk::get()
{
	return commandBuffer;
}