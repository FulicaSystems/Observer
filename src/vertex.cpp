#include "vertex.hpp"

VkVertexInputBindingDescription Vertex::getBindingDescription()
{
	// describe the buffer data
	VkVertexInputBindingDescription desc = {
		.binding = 0,
		.stride = sizeof(Vertex),
		// update every vertex (opposed to VK_VERTEX_INPUT_RATE_INSTANCE)
		.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
	};

	return desc;
}

std::array<VkVertexInputAttributeDescription, 2> Vertex::getAttributeDescription()
{
	// attribute pointer
	std::array<VkVertexInputAttributeDescription, 2> desc;
	desc[0] = {
		.location = 0,
		.binding = 0,
		.format = VK_FORMAT_R32G32B32_SFLOAT,
		.offset = offsetof(Vertex, position)
	};
	desc[1] = {
		.location = 1,
		.binding = 0,
		.format = VK_FORMAT_R32G32B32A32_SFLOAT,
		.offset = offsetof(Vertex, color)
	};
	return desc;
}