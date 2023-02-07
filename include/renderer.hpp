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
	// TODO : make the pipeline independant in order to create multiple different pipelines
	GraphicsPipeline pipeline;

public:
	LowRenderer low;
	LogicalDevice device;

	Renderer();

	void create() override;
	void destroy() override;

	void renderScene(const std::vector<VkBuffer>& vbos);
};