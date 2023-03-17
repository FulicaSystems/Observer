#include "graphicsdevice.hpp"
#include "lowrenderer.hpp"

#include "commandpool.hpp"

void CommandPool::vulkanCommandPool()
{
	VkDevice vkdevice = device->vkdevice;
	PhysicalDevice pdevice = device->pdevice.vkpdevice;

	VkQueueFamilyIndices indices = pdevice.findQueueFamilies(api->surface);

	VkCommandPoolCreateInfo createInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		.queueFamilyIndex = indices.graphicsFamily.value()
	};

	if (vkCreateCommandPool(vkdevice, &createInfo, nullptr, &commandPool) != VK_SUCCESS)
		throw std::exception("Failed to create command pool");
}

CommandBuffer CommandPool::createFloatingCommandBuffer()
{
	VkDevice ldevice = device->vkdevice;

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
	return cbos.emplace_back(createFloatingCommandBuffer());
}

CommandBuffer& CommandPool::getCmdBufferByIndex(const int index)
{
	return cbos[index];
}

void CommandPool::create(LowRenderer* api, LogicalDevice* device)
{
	Super::create(api, device);

	vulkanCommandPool();
}

void CommandPool::destroy()
{
	for (int i = 0; i < cbos.size(); ++i)
	{
		destroyFloatingCommandBuffer(cbos[i]);
	}
	cbos.clear();

	vkDestroyCommandPool(device->vkdevice, commandPool, nullptr);
}

void CommandPool::destroyFloatingCommandBuffer(CommandBuffer& cbo)
{
	vkFreeCommandBuffers(device->vkdevice, commandPool, 1, &cbo.getVkBuffer());
}

void CommandPool::destroyCommandBuffer(const int index)
{
	destroyFloatingCommandBuffer(cbos[index]);
	cbos.erase(cbos.begin() + index);
}