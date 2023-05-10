#include "vmahelper.hpp"
#include "allocator.hpp"
#include "commandbuffer.hpp"

#include "renderer.hpp"

void Renderer::destroyFloatingBufferObject(VertexBuffer& vbo)
{
	vkDestroyBuffer(device.vkdevice, vbo.buffer, nullptr);
	allocator->destroyBufferObjectMemory(vbo);
}

void Renderer::destroyBufferObject(int index)
{
	destroyFloatingBufferObject(*vbos[index]);
	vbos.erase(vbos.begin() + index);
}

VertexBuffer& Renderer::createVertexBufferObject(uint32_t vertexNum, const Vertex* vertices)
{
	// this buffer is a CPU accessible buffer (temporary buffer to later load the data to the GPU)
	std::shared_ptr<VertexBuffer> stagingVBO = createFloatingBufferObject(vertexNum,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,	// used for memory transfer operation
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		true);

	// populating the CPU accessible buffer
	populateBufferObject(*stagingVBO, vertices);

	// creating a device (GPU) local buffer (on the specific device of the renderer)
	// store this vertex buffer to keep a reference
	std::shared_ptr<VertexBuffer>& vbo = createBufferObject(vertexNum,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,	// memory transfer operation
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	// copying the staging buffer data into the device local buffer

	// using a command buffer to transfer the data
	CommandBuffer cbo = commandPool.createFloatingCommandBuffer();

	// copy the staging buffer (CPU accessible) into the GPU buffer (GPU memory)
	cbo.beginRecord(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	VkBufferCopy copyRegion = {
		.srcOffset = 0,
		.dstOffset = 0,
		.size = stagingVBO->bufferSize
	};
	vkCmdCopyBuffer(cbo.getVkBuffer(), stagingVBO->buffer, vbo->buffer, 1, &copyRegion);

	cbo.endRecord();

	VkSubmitInfo submitInfo = {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.commandBufferCount = 1,
		.pCommandBuffers = &cbo.getVkBuffer()
	};
	device.submitCommandToGraphicsQueue(submitInfo);
	device.waitGraphicsQueue();

	commandPool.destroyFloatingCommandBuffer(cbo);
	destroyFloatingBufferObject(*stagingVBO);

	return *vbo;
}

void Renderer::initRenderer()
{
	// create the rendering instance first using api.initGraphicsAPI()
	device.create(&api, nullptr);
	commandPool.create(&api, &device);
	pipeline.create(&api, &device);

	allocator = new MyAllocator();
	allocator->create(&api, &device);

	// default command buffer
	commandPool.createCommandBuffer();
}

void Renderer::terminateRenderer()
{
	pipeline.destroy();
	commandPool.destroy();

	for (int i = 0; i < vbos.size(); ++i)
	{
		destroyFloatingBufferObject(*vbos[i]);
	}
	vbos.clear();

	allocator->destroy();
	delete allocator;

	device.destroy();

	api.terminateGraphicsAPI();
}

[[nodiscard]] std::shared_ptr<VertexBuffer> Renderer::createFloatingBufferObject(uint32_t vertexNum,
	VkBufferUsageFlags usage,
	VkMemoryPropertyFlags memProperties,
	bool mappable)
{
	// out buffer object
	std::shared_ptr<VertexBuffer> outVbo = VertexBuffer::createNew();
	outVbo->bufferSize = sizeof(Vertex) * (size_t)vertexNum;
	outVbo->vertexNum = vertexNum;

	VkBufferCreateInfo createInfo = {
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.flags = 0,
		.size = (VkDeviceSize)outVbo->bufferSize,
		.usage = usage,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE
	};

	allocator->allocateBufferObjectMemory(createInfo, *outVbo, memProperties, mappable);

	return outVbo;
}

[[nodiscard]] std::shared_ptr<VertexBuffer>& Renderer::createBufferObject(uint32_t vertexNum,
	VkBufferUsageFlags usage,
	VkMemoryPropertyFlags memProperties,
	bool mappable)
{
	std::shared_ptr<VertexBuffer> newVBO = createFloatingBufferObject(vertexNum, usage, memProperties, mappable);
	return vbos.emplace_back(newVBO);
}

void Renderer::populateBufferObject(VertexBuffer& vbo, const Vertex* vertices)
{
	// populating the VBO (using a CPU accessible memory)
	allocator->mapMemory(vbo.alloc, &vbo.vertices);

	// TODO : flush memory
	memcpy(vbo.vertices, vertices, vbo.bufferSize);
	// TODO : invalidate memory before reading in the pipeline

	allocator->unmapMemory(vbo.alloc);
}

void Renderer::render()
{
	pipeline.drawFrame(commandPool.getCmdBufferByIndex(0), vbos);
}