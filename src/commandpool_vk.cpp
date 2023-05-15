#include <stdexcept>

#include "graphicsdevice_vk.hpp"
#include "lowrenderer_vk.hpp"

#include "commandpool_vk.hpp"

void CommandPool_Vk::vulkanCommandPool()
{
	VkDevice vkdevice = ((LogicalDevice_Vk*)device)->vkdevice;
	PhysicalDevice pdevice = ((LogicalDevice_Vk*)device)->pdevice.vkpdevice;

	VkQueueFamilyIndices indices = pdevice.findQueueFamilies(((LowRenderer_Vk*)api)->surface);

	VkCommandPoolCreateInfo createInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		.queueFamilyIndex = indices.graphicsFamily.value()
	};

	if (vkCreateCommandPool(vkdevice, &createInfo, nullptr, &commandPool) != VK_SUCCESS)
		throw std::runtime_error("Failed to create command pool");
}

CommandBuffer_Vk CommandPool_Vk::createFloatingCommandBuffer()
{
	VkDevice ldevice = ((LogicalDevice_Vk*)device)->vkdevice;

	VkCommandBufferAllocateInfo allocInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.commandPool = commandPool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = 1
	};

	CommandBuffer_Vk outCbo;
	if (vkAllocateCommandBuffers(ldevice, &allocInfo, &outCbo.get()) != VK_SUCCESS)
		throw std::runtime_error("Failed to allocate command buffers");

	return outCbo;
}

CommandBuffer_Vk& CommandPool_Vk::createCommandBuffer()
{
	return cbos.emplace_back(createFloatingCommandBuffer());
}

CommandBuffer_Vk& CommandPool_Vk::getCmdBufferByIndex(const int index)
{
	return cbos[index];
}

void CommandPool_Vk::create(ILowRenderer* api, ILogicalDevice* device)
{
	Super::create(api, device);

	vulkanCommandPool();
}

void CommandPool_Vk::destroy()
{
	for (int i = 0; i < cbos.size(); ++i)
	{
		destroyFloatingCommandBuffer(cbos[i]);
	}
	cbos.clear();

	vkDestroyCommandPool(((LogicalDevice_Vk*)device)->vkdevice, commandPool, nullptr);
}

void CommandPool_Vk::destroyFloatingCommandBuffer(CommandBuffer_Vk& cbo)
{
	vkFreeCommandBuffers(((LogicalDevice_Vk*)device)->vkdevice, commandPool, 1, &cbo.get());
}

void CommandPool_Vk::destroyCommandBuffer(const int index)
{
	destroyFloatingCommandBuffer(cbos[index]);
	cbos.erase(cbos.begin() + index);
}