#pragma once

#include <glad/vulkan.h>

#include "graphicsobject.hpp"

class LogicalDevice;

class VertexBuffer : public IGraphicsObject
{
private:
	// the logical device in which this buffer is created and allocated
	LogicalDevice& ldevice;

	VkBuffer vbo;
	VkDeviceMemory vboMemory;

	VertexBuffer(LogicalDevice& ldevice);

public:
	static VertexBuffer createBufferObject(LogicalDevice& ldevice,
		VkDeviceSize bufferSize,
		VkBufferUsageFlags usage,
		VkMemoryPropertyFlags memProperties);

	void create() override {}
	void destroy() override;
};