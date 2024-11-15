#pragma once

enum class GraphicsApiE
{
	NONE = 0,
	OPENGL = 1 << 0,
	VULKAN = 1 << 1,
	DIRECT3D11 = 1 << 2,
	DIRECT3D12 = 1 << 3,
};