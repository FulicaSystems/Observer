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
	GraphicsPipeline pipeline;

public:
	LowRenderer low;

	Renderer();

	void create() override;
	void destroy() override;

	void render();
};