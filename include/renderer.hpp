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

	// every created buffer objects
	std::unordered_map<int, VertexBuffer> vbos;

public:
	LowRenderer low;

	LogicalDevice ldevice;
	CommandPool commandPool;

	Renderer();

	void create() override;
	void destroy() override;

	VertexBuffer createFloatingBufferObject(uint32_t vertexNum,
		VkBufferUsageFlags usage,
		VkMemoryPropertyFlags memProperties);
	VertexBuffer& createBufferObject(uint32_t vertexNum,
		VkBufferUsageFlags usage,
		VkMemoryPropertyFlags memProperties);

	void populateBufferObject(VertexBuffer& vbo, Vertex* vertices);

	void destroyBufferObject(int index);
	void destroyBufferObject(VertexBuffer& vbo);

	void render();
};