#include <exception>
#include <vector>
#include <iostream>

#include "format.hpp"
#include "lowrenderer.hpp"

#include "application.hpp"

Application::Application()
{
	glfwInit();
	windowInit();

	LowRenderer::create(window);
	rdr.create();
}

void Application::destroy()
{
	rdr.destroy();
	LowRenderer::destroy();

	glfwDestroyWindow(window);
	glfwTerminate();
}

void Application::windowInit()
{
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(Format::height, Format::width, "Vulkan window", nullptr, nullptr);
	glfwGetFramebufferSize(window, &Format::framebufferHeight, &Format::framebufferWidth);
}

void Application::loop()
{
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		//draw frame

		rdr.pipeline.drawFrame();
	}
}