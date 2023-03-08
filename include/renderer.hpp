#pragma once

#include <unordered_map>

#include "graphicsobject.hpp"

#include "lowrenderer.hpp"

#include "graphicsdevice.hpp"
#include "graphicspipeline.hpp"
#include "commandpool.hpp"

#include "vertex.hpp"
#include "vertexbuffer.hpp"

/**
 * High level renderer.
 */
class Renderer : public IGraphicsObject
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
	std::unordered_map<int, VertexBuffer> vbos;

public:
	LowRenderer low;

	LogicalDevice ldevice;
	CommandPool commandPool;

	Renderer();

	void create() override;
	void destroy() override;

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
	void createVertexBufferObject(uint32_t vertexNum, Vertex* vertices);


	// rendering

	/**
	 * Rendering a single frame.
	 */
	void render();
};