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

	// load the vulkan symbols using GLFW and glad
	if (!glfwVulkanSupported())
		throw std::exception("GLFW failed to find the Vulkan loader");

	if (!gladLoaderLoadVulkan(nullptr, nullptr, nullptr))
		throw std::exception("Unable to load Vulkan symbols");

	// create the Vulkan instance first
	rdr.low.create();

	// create a surface using the instance
	if (glfwCreateWindowSurface(rdr.low.instance, window, nullptr, &rdr.low.surface) != VK_SUCCESS)
		throw std::exception("Failed to create window surface");

	rdr.create();
}

Application::~Application()
{
	rdr.destroy();

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