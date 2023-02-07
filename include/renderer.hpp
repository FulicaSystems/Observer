#pragma once

#include <unordered_map>

#include "graphicsobject.hpp"

#include "lowrenderer.hpp"

#include "graphicsdevice.hpp"
#include "graphicspipeline.hpp"

#include "vertex.hpp"
#include "vertexbuffer.hpp"

/**
 * High level renderer.
 */
class Renderer : public IGraphicsObject
{
private:
	LogicalDevice ldevice;
	// TODO : make the pipeline independant in order to make different pipelines
	GraphicsPipeline pipeline;

	// every created buffer objects
	std::unordered_map<int, VertexBuffer> vbos;

	void destroyBufferObject(VertexBuffer& vbo);

public:
	LowRenderer low;

	Renderer();

	void create() override;
	void destroy() override;

	VertexBuffer& createBufferObject(uint32_t vertexNum,
		VkBufferUsageFlags usage,
		VkMemoryPropertyFlags memProperties);

	void populateBufferObject(VertexBuffer& vbo, Vertex* vertices);

	void render();
};