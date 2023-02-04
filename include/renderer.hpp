#pragma once

#include "graphicsdevice.hpp"
#include "graphicspipeline.hpp"

class Renderer : public IGraphicsObject
{
private:
	LogicalDevice device;

public:
	GraphicsPipeline pipeline;

	Renderer();

	void create() override;
	void destroy() override;
};