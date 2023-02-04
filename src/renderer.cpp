#include "renderer.hpp"
#include "renderer.hpp"

Renderer::Renderer()
	: pipeline(device)
{
}

void Renderer::create()
{
	device.create();
	pipeline.create();
}

void Renderer::destroy()
{
	pipeline.destroy();
	device.destroy();
}