#pragma once

#include <array>

#include <glad/vulkan.h>

#include "mathematics.hpp"
#include "types/color.hpp"

class Vertex
{
public:
	// 2 attributes
	vec3 position;
	//vec3 normal;
	Color color;

	// binding the data to the vertex shader
	static VkVertexInputBindingDescription getBindingDescription();

	// 2 attributes descripction
	static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescription();
};