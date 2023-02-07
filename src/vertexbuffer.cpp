#include <exception>

#include "graphicsdevice.hpp"
#include "vertex.hpp"

#include "vertexbuffer.hpp"

VertexBuffer::VertexBuffer(LogicalDevice& ldevice)
	: ldevice(ldevice)
{
}

VertexBuffer VertexBuffer::createBufferObject(LogicalDevice& ldevice,
	uint32_t vertexNum,
	VkBufferUsageFlags usage,
	VkMemoryPropertyFlags memProperties)
{
	// out buffer object
	VertexBuffer bo = ldevice;
	bo.bufferSize = sizeof(Vertex) * (size_t)vertexNum;

	const VkDevice& device = ldevice.getVkLDevice();

	// TODO : staging buffers for better performance (https://vulkan-tutorial.com/en/Vertex_buffers/Staging_buffer)
	VkBufferCreateInfo createInfo = {
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.flags = 0,
		.size = (VkDeviceSize)bo.bufferSize,
		.usage = usage,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE
	};

	if (vkCreateBuffer(device, &createInfo, nullptr, &bo.buffer) != VK_SUCCESS)
		throw std::exception("Failed to create vertex buffer");

	VkMemoryRequirements memReq;
	vkGetBufferMemoryRequirements(device, bo.buffer, &memReq);

	// VRAM heap
	VkMemoryAllocateInfo allocInfo = {
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.allocationSize = memReq.size,
		.memoryTypeIndex = ldevice.getPDevice().findMemoryType(memReq.memoryTypeBits, memProperties)
	};

	if (vkAllocateMemory(device, &allocInfo, nullptr, &bo.memory) != VK_SUCCESS)
		throw std::exception("Failed to allocate vertex buffer memory");

	// TODO : use offset
	vkBindBufferMemory(device, bo.buffer, bo.memory, 0);

	return bo;
}

void VertexBuffer::destroy()
{
	const VkDevice& device = ldevice.getVkLDevice();
	vkDeviceWaitIdle(device);

	vkDestroyBuffer(device, buffer, nullptr);
	vkFreeMemory(device, memory, nullptr);
}

void VertexBuffer::populate(Vertex* vertices)
{
	const VkDevice& device = ldevice.getVkLDevice();

	// populating the VBO (using a CPU accessible memory)
	void* data;
	vkMapMemory(device, memory, 0, (VkDeviceSize)bufferSize, 0, &data);
	// TODO : flush memory
	memcpy(data, vertices, bufferSize);
	// TODO : invalidate memory before reading in the pipeline
	vkUnmapMemory(device, memory);
}