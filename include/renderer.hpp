#pragma once

#include "graphicsdevice.hpp"
#include "graphicspipeline.hpp"

class Renderer
{
private:
	LogicalDevice device;

public:
	GraphicsPipeline pipeline;

	Renderer();

	void create();
	void destroy();
};
