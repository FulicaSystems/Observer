#pragma once

#include <array>

#include "mathematics.hpp"
#include "types/color.hpp"

struct Vertex
{
	// 2 attributes
	vec3 position;
	//vec3 normal;
	Color color;

	// TODO : move those functions to a Vulkan specific file

	// binding the data to the vertex shader
	static struct VkVertexInputBindingDescription getBindingDescription();

	// 2 attributes descripction
	static std::array<struct VkVertexInputAttributeDescription, 2> getAttributeDescription();
};