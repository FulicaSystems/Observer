#pragma once

#include <deque>

#include "vertex.hpp"
#include "vertexbuffer.hpp"

enum class EGraphicsAPI
{
	OPENGL = 0,
	VULKAN = 1
};

/**
 * High level renderer.
 */
class Renderer
{
private:
	// TODO : make the pipeline independant in order to make different pipelines
	class IGraphicsPipeline* pipeline = nullptr;

	class IMemoryAllocator* allocator = nullptr;

	// every created buffer objects
	std::deque<std::shared_ptr<VertexBuffer>> vbos;

public:
	EGraphicsAPI graphicsApi;

	class ILowRenderer* api = nullptr;
	class ILogicalDevice* device = nullptr;

	class ICommandPool* commandPool = nullptr;

	Renderer(const EGraphicsAPI graphicsApi = EGraphicsAPI::VULKAN);
	~Renderer();

	void initRenderer();
	void terminateRenderer();

	// vertex buffer object

	[[nodiscard]] std::shared_ptr<VertexBuffer> createFloatingBufferObject(uint32_t vertexNum,
		VkBufferUsageFlags usage,
		VkMemoryPropertyFlags memProperties,
		bool mappable = false);
	[[nodiscard]] std::shared_ptr<VertexBuffer>& createBufferObject(uint32_t vertexNum,
		VkBufferUsageFlags usage,
		VkMemoryPropertyFlags memProperties,
		bool mappable = false);

	void populateBufferObject(VertexBuffer& vbo, const Vertex* vertices);

	void destroyFloatingBufferObject(VertexBuffer& vbo);
	void destroyBufferObject(int index);

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