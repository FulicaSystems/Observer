#pragma once

#include "graphicsobject.hpp"

#include "lowrenderer.hpp"
#include "graphicsdevice.hpp"
#include "graphicspipeline.hpp"

/**
 * High level renderer.
 */
class Renderer : public IGraphicsObject
{
private:
	LogicalDevice device;

public:
	LowRenderer low;
	GraphicsPipeline pipeline;

	Renderer();

	void create() override;
	void destroy() override;
};