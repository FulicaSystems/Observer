#pragma once

#include <deque>
#include <memory>

#include "vertexbuffer.hpp"

#include "graphicsapi.hpp"

/**
 * High level renderer.
 */
class Renderer
{
private:
	// every created buffer objects
	std::deque<std::shared_ptr<IVertexBuffer>> vbos;

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
	IVertexBuffer& addVBO(std::shared_ptr<IVertexBuffer> vbo);


	// rendering

	/**
	 * Rendering a single frame.
	 */
	void render();
};