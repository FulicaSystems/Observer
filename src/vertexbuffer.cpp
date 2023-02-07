#include <exception>

#include "graphicsdevice.hpp"

#include "vertexbuffer.hpp"

VertexBuffer::VertexBuffer(LogicalDevice& ldevice)
	: ldevice(ldevice)
{
}

VertexBuffer VertexBuffer::createBufferObject(LogicalDevice& ldevice,
	VkDeviceSize bufferSize,
	VkBufferUsageFlags usage,
	VkMemoryPropertyFlags memProperties)
{
	// out buffer object
	VertexBuffer bo = ldevice;

	const VkDevice& device = ldevice.getVkLDevice();

	// TODO : staging buffers for better performance (https://vulkan-tutorial.com/en/Vertex_buffers/Staging_buffer)
	VkBufferCreateInfo createInfo = {
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.flags = 0,
		.size = bufferSize,
		.usage = usage,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE
	};

	if (vkCreateBuffer(device, &createInfo, nullptr, &bo.vbo) != VK_SUCCESS)
		throw std::exception("Failed to create vertex buffer");

	VkMemoryRequirements memReq;
	vkGetBufferMemoryRequirements(device, bo.vbo, &memReq);

	// VRAM heap
	VkMemoryAllocateInfo allocInfo = {
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.allocationSize = memReq.size,
		.memoryTypeIndex = ldevice.getPDevice().findMemoryType(memReq.memoryTypeBits, memProperties)
	};

	if (vkAllocateMemory(device, &allocInfo, nullptr, &bo.vboMemory) != VK_SUCCESS)
		throw std::exception("Failed to allocate vertex buffer memory");

	vkBindBufferMemory(device, bo.vbo, bo.vboMemory, 0);

	return bo;
}

void VertexBuffer::destroy()
{
	const VkDevice& device = ldevice.getVkLDevice();

	vkDestroyBuffer(device, vbo, nullptr);
	vkFreeMemory(device, vboMemory, nullptr);
}