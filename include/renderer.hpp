#pragma once

#include <deque>

#include "graphicsapi.hpp"

#include "vertex.hpp"
#include "vertexbuffer.hpp"

/**
 * High level renderer.
 */
class Renderer
{
private:
	// every created buffer objects
	std::deque<std::shared_ptr<VertexBuffer>> vbos;

public:
	EGraphicsAPI graphicsApi;

	class ILowRenderer* api = nullptr;
	class ILogicalDevice* device = nullptr;

	// TODO : make the pipeline independant in order to make different pipelines
	class IGraphicsPipeline* pipeline = nullptr;

	class IMemoryAllocator* allocator = nullptr;

	class ICommandPool* commandPool = nullptr;

	Renderer(const EGraphicsAPI graphicsApi = EGraphicsAPI::VULKAN);
	~Renderer();

	void initRenderer();
	void terminateRenderer();


	// vertex buffer object

	/**
	 * Create a vertex buffer and store it to the vertex buffer container.
	 */
	VertexBuffer& createVertexBufferObject(uint32_t vertexNum, const Vertex* vertices);


	// rendering

	/**
	 * Rendering a single frame.
	 */
	void render();
};