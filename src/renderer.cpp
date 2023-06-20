#include "vertex.hpp"
#include "vertexbuffer.hpp"
#include "shader.hpp"

#include "memorymanager.hpp"
#include "commandbuffer_vk.hpp"
#include "commandpool_vk.hpp"
#include "graphicsdevice_vk.hpp"
#include "graphicspipeline_vk.hpp"
#include "lowrenderer_vk.hpp"

#include "lowrenderer_gl.hpp"

#include "resourcesmanager.hpp"

#include "renderer.hpp"

Renderer::Renderer(const EGraphicsAPI graphicsApi)
	: graphicsApi(graphicsApi)
{
	switch (this->graphicsApi)
	{
	case EGraphicsAPI::OPENGL:
	{
		api = new LowRenderer_Gl();

		break;
	}
	case EGraphicsAPI::VULKAN:
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

		break;
	}
	default:
		throw std::runtime_error("Invalid graphics API");
	}
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
	api->highRenderer = this;

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
		api->destroyBufferObject(*vbos[i]);
	}
	vbos.clear();
	ResourcesManager::clearAllResources();

	allocator->destroy();

	((LogicalDevice_Vk*)device)->destroy();

	((LowRenderer_Vk*)api)->terminateGraphicsAPI();
}

IVertexBuffer& Renderer::addVBO(std::shared_ptr<IVertexBuffer> vbo)
{
	return *vbos.emplace_back(vbo);
}

void Renderer::render()
{
	((GraphicsPipeline_Vk*)pipeline)->drawFrame(((CommandPool_Vk*)commandPool)->getCmdBufferByIndex(0), vbos);
}