#include "commandpool.hpp"
#include "graphicsdevice.hpp"

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

CommandBuffer CommandPool::createFloatingCommandBuffer()
{
	VkDevice ldevice = device.getVkLDevice();

	VkCommandBufferAllocateInfo allocInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.commandPool = commandPool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = 1
	};

	CommandBuffer outCbo;
	if (vkAllocateCommandBuffers(ldevice, &allocInfo, &outCbo.getVkBuffer()) != VK_SUCCESS)
		throw std::exception("Failed to allocate command buffers");

	return outCbo;
}

CommandBuffer& CommandPool::createCommandBuffer()
{
	int index = cbos.size();
	cbos[index] = createFloatingCommandBuffer();
	return cbos[index];
}

CommandBuffer& CommandPool::getCmdBufferByIndex(const int index)
{
	return cbos[index];
}

CommandPool::CommandPool(LogicalDevice& device)
	: device(device)
{
}

void CommandPool::create()
{
	vulkanCommandPool();
}

void CommandPool::destroy()
{
	for (int i = 0; i < cbos.size(); ++i)
	{
		destroyFloatingCommandBuffer(cbos[i]);
	}
	cbos.clear();

	vkDestroyCommandPool(device.getVkLDevice(), commandPool, nullptr);
}

void CommandPool::destroyFloatingCommandBuffer(CommandBuffer& cbo)
{
	vkFreeCommandBuffers(device.getVkLDevice(), commandPool, 1, &cbo.getVkBuffer());
}

void CommandPool::destroyCommandBuffer(const int index)
{
	destroyFloatingCommandBuffer(cbos[index]);
	cbos.erase(index);
}