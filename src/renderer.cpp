#include "commandbuffer_vk.hpp"
#include "commandpool_vk.hpp"
#include "graphicsdevice_vk.hpp"
#include "graphicspipeline_vk.hpp"
#include "lowrenderer_vk.hpp"

#include "renderer.hpp"

#ifdef USE_VMA
#include "vmahelper.hpp"
#else
#include "allocator_vk.hpp"
#endif

void Renderer::destroyFloatingBufferObject(VertexBuffer& vbo)
{
	VertexBufferDesc_Vk* desc = (VertexBufferDesc_Vk*)vbo.localDesc;
	vkDestroyBuffer(((LogicalDevice_Vk*)device)->vkdevice, desc->buffer, nullptr);
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
	CommandBuffer_Vk cbo = ((CommandPool_Vk*)commandPool)->createFloatingCommandBuffer();

	// copy the staging buffer (CPU accessible) into the GPU buffer (GPU memory)
	cbo.beginRecord(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	VkBufferCopy copyRegion = {
		.srcOffset = 0,
		.dstOffset = 0,
		.size = stagingVBO->bufferSize
	};
	VertexBufferDesc_Vk* stagingDesc = (VertexBufferDesc_Vk*)stagingVBO->localDesc;
	VertexBufferDesc_Vk* desc = (VertexBufferDesc_Vk*)vbo->localDesc;
	vkCmdCopyBuffer(cbo.get(), stagingDesc->buffer, desc->buffer, 1, &copyRegion);

	cbo.endRecord();

	VkSubmitInfo submitInfo = {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.commandBufferCount = 1,
		.pCommandBuffers = &cbo.get()
	};
	((LogicalDevice_Vk*)device)->submitCommandToGraphicsQueue(submitInfo);
	((LogicalDevice_Vk*)device)->waitGraphicsQueue();

	((CommandPool_Vk*)commandPool)->destroyFloatingCommandBuffer(cbo);
	destroyFloatingBufferObject(*stagingVBO);

	return *vbo;
}

Renderer::Renderer(const EGraphicsAPI graphicsApi)
	: graphicsApi(graphicsApi)
{
	// TODO : no naked new (clean creation)
	pipeline = new GraphicsPipeline_Vk();
#ifdef USE_VMA
	allocator = new Allocator_VMA();
#else
	allocator = new MyAllocator_Vk();
#endif
	api = new LowRenderer_Vk();
	device = new LogicalDevice_Vk();
	commandPool = new CommandPool_Vk();
}

Renderer::~Renderer()
{
	delete pipeline;
	delete allocator;
	delete api;
	delete device;
	delete commandPool;
}

void Renderer::initRenderer()
{
	switch (graphicsApi)
	{
	case EGraphicsAPI::OPENGL:
	{
		break;
	}
	case EGraphicsAPI::VULKAN:
	{
		// create the rendering instance first using api.initGraphicsAPI()
		((LogicalDevice_Vk*)device)->create(api, nullptr);
		((CommandPool_Vk*)commandPool)->create(api, device);
		((GraphicsPipeline_Vk*)pipeline)->create(api, device);

		allocator->create(api, device);

		// default command buffer
		((CommandPool_Vk*)commandPool)->createCommandBuffer();

		break;
	}
	default:
		throw std::runtime_error("Invalid graphics API");
	}
}

void Renderer::terminateRenderer()
{
	((GraphicsPipeline_Vk*)pipeline)->destroy();
	((CommandPool_Vk*)commandPool)->destroy();

	for (int i = 0; i < vbos.size(); ++i)
	{
		destroyFloatingBufferObject(*vbos[i]);
	}
	vbos.clear();

	allocator->destroy();

	((LogicalDevice_Vk*)device)->destroy();

	((LowRenderer_Vk*)api)->terminateGraphicsAPI();
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
	VertexBufferDesc_Vk* desc = (VertexBufferDesc_Vk*)vbo.localDesc;

	// populating the VBO (using a CPU accessible memory)
	allocator->mapMemory(desc->alloc, &vbo.vertices);

	// TODO : flush memory
	memcpy(vbo.vertices, vertices, vbo.bufferSize);
	// TODO : invalidate memory before reading in the pipeline

	allocator->unmapMemory(desc->alloc);
}

void Renderer::render()
{
	((GraphicsPipeline_Vk*)pipeline)->drawFrame(((CommandPool_Vk*)commandPool)->getCmdBufferByIndex(0), vbos);
}