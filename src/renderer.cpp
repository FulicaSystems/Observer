#include "renderer.hpp"
#include "renderer.hpp"

Renderer::Renderer()
	: device(low), pipeline(device)
{
}

void Renderer::create()
{
	// do not forget to call low.create() first to create the rendering instance
	device.create();
	pipeline.create();
}

void Renderer::destroy()
{
	pipeline.destroy();
	device.destroy();
	low.destroy();
}