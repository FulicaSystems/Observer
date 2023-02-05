#include "renderer.hpp"
#include "renderer.hpp"

Renderer::Renderer()
	: device(low), pipeline(device)
{
}

void Renderer::create()
{
	// create the rendering instance first using low.create()
	device.create();
	pipeline.create();
}

void Renderer::destroy()
{
	pipeline.destroy();
	device.destroy();
	low.destroy();
}

void Renderer::render()
{
	pipeline.drawFrame();
}