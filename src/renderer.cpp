#include "renderer.hpp"

void Renderer::destroyBufferObject(int index)
{
	const VkDevice& device = ldevice.getVkLDevice();
	vkDestroyBuffer(device, vbos[index].buffer, nullptr);
	vkFreeMemory(device, vbos[index].memory, nullptr);

	vbos.erase(index);
}

void Renderer::destroyBufferObject(VertexBuffer& vbo)
{
	const VkDevice& device = ldevice.getVkLDevice();
	vkDestroyBuffer(device, vbo.buffer, nullptr);
	vkFreeMemory(device, vbo.memory, nullptr);
}

Renderer::Renderer()
	: ldevice(low), commandPool(ldevice), pipeline(ldevice)
{
}

void Renderer::create()
{
	// create the rendering instance first using low.create()
	ldevice.create();
	commandPool.create();
	pipeline.create();

	commandPool.createCommandBuffer();
}

void Renderer::destroy()
{
	pipeline.destroy();
	commandPool.destroy();

	for (int i = 0; i < vbos.size(); ++i)
	{
		destroyBufferObject(vbos[i]);
	}
	vbos.clear();

	ldevice.destroy();
	low.destroy();
}

VertexBuffer Renderer::createFloatingBufferObject(uint32_t vertexNum, VkBufferUsageFlags usage, VkMemoryPropertyFlags memProperties)
{
	// out buffer object
	VertexBuffer outVbo;
	outVbo.bufferSize = sizeof(Vertex) * (size_t)vertexNum;
	outVbo.vertexNum = vertexNum;

	const VkDevice& device = ldevice.getVkLDevice();

	VkBufferCreateInfo createInfo = {
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.flags = 0,
		.size = (VkDeviceSize)outVbo.bufferSize,
		.usage = usage,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE
	};

	if (vkCreateBuffer(device, &createInfo, nullptr, &outVbo.buffer) != VK_SUCCESS)
		throw std::exception("Failed to create vertex buffer");

	VkMemoryRequirements memReq;
	vkGetBufferMemoryRequirements(device, outVbo.buffer, &memReq);

	// VRAM heap
	VkMemoryAllocateInfo allocInfo = {
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.allocationSize = memReq.size,
		.memoryTypeIndex = ldevice.getPDevice().findMemoryType(memReq.memoryTypeBits, memProperties)
	};

	if (vkAllocateMemory(device, &allocInfo, nullptr, &outVbo.memory) != VK_SUCCESS)
		throw std::exception("Failed to allocate vertex buffer memory");

	// TODO : use offset
	vkBindBufferMemory(device, outVbo.buffer, outVbo.memory, 0);

	return outVbo;
}

VertexBuffer& Renderer::createBufferObject(uint32_t vertexNum, VkBufferUsageFlags usage, VkMemoryPropertyFlags memProperties)
{	
	int index = vbos.size();
	vbos[index] = createFloatingBufferObject(vertexNum, usage, memProperties);
	return vbos[index];
}

void Renderer::populateBufferObject(VertexBuffer& vbo, Vertex* vertices)
{
	const VkDevice& device = ldevice.getVkLDevice();

	// populating the VBO (using a CPU accessible memory)
	vkMapMemory(device, vbo.memory, 0, (VkDeviceSize)vbo.bufferSize, 0, &vbo.vertices);
	// TODO : flush memory
	memcpy(vbo.vertices, vertices, vbo.bufferSize);
	// TODO : invalidate memory before reading in the pipeline
	vkUnmapMemory(device, vbo.memory);
}

void Renderer::render()
{
	pipeline.drawFrame(commandPool.getCmdBufferByIndex(0), vbos);
}