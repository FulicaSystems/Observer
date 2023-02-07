#pragma once

#include <glad/vulkan.h>

#include "graphicsobject.hpp"

class LogicalDevice;
class Vertex;

class VertexBuffer : public IGraphicsObject
{
private:
	// the logical device in which this buffer is created and allocated
	LogicalDevice& ldevice;

	// buffer size
	size_t bufferSize = 0;

	// memory allocated on the GPU heap
	VkDeviceMemory memory;

	VertexBuffer(LogicalDevice& ldevice);

public:
	// vertex buffer object
	VkBuffer buffer;

	static VertexBuffer createBufferObject(LogicalDevice& ldevice,
		uint32_t vertexNum,
		VkBufferUsageFlags usage,
		VkMemoryPropertyFlags memProperties);

	void create() override {}
	void destroy() override;

	void populate(Vertex* vertices);
};