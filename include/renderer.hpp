#pragma once

#include <deque>

#include "lowrenderer.hpp"

#include "graphicsdevice.hpp"
#include "graphicspipeline.hpp"
#include "commandpool.hpp"

#include "vertex.hpp"
#include "vertexbuffer.hpp"

/**
 * High level renderer.
 */
class Renderer
{
private:
	// TODO : make the pipeline independant in order to make different pipelines
	GraphicsPipeline pipeline;

#if false
	MemoryAllocator allocator;
#else
	VmaAllocator allocator;
#endif

	// every created buffer objects
	std::deque<VertexBuffer> vbos;

public:
	LowRenderer api;
	LogicalDevice device;

	CommandPool commandPool;

	void initRenderer();
	void terminateRenderer();

	// vertex buffer object

	VertexBuffer createFloatingBufferObject(uint32_t vertexNum,
		VkBufferUsageFlags usage,
		VkMemoryPropertyFlags memProperties,
		bool mappable = false);
	VertexBuffer& createBufferObject(uint32_t vertexNum,
		VkBufferUsageFlags usage,
		VkMemoryPropertyFlags memProperties,
		bool mappable = false);

	void populateBufferObject(VertexBuffer& vbo, Vertex* vertices);

	void destroyFloatingBufferObject(VertexBuffer& vbo);
	void destroyBufferObject(int index);

	/**
	 * Create a vertex buffer and store it to the vertex buffer container.
	 */
	VertexBuffer& createVertexBufferObject(uint32_t vertexNum, Vertex* vertices);


	// rendering

	/**
	 * Rendering a single frame.
	 */
	void render();
};