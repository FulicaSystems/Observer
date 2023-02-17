#include "commandpool.hpp"

void CommandPool::vulkanCommandPool()
{
	VkDevice ldevice = device.getVkLDevice();
	PhysicalDevice pdevice = device.getPDevice();

	VkQueueFamilyIndices indices = pdevice.findQueueFamilies(device.low.surface);

	VkCommandPoolCreateInfo createInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		.queueFamilyIndex = indices.graphicsFamily.value()
	};

	if (vkCreateCommandPool(ldevice, &createInfo, nullptr, &commandPool) != VK_SUCCESS)
		throw std::exception("Failed to create command pool");
}

void CommandPool::create()
{
}

void CommandPool::destroy()
{
}