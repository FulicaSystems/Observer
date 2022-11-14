#pragma once

#include "graphicsdevice.hpp"
#include "graphicspipeline.hpp"

class Renderer
{
private:
	LogicalDevice device;
	GraphicsPipeline pipeline;

public:
	void create();
	void destroy();
};
