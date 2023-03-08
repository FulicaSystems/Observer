#include <exception>
#include <vector>
#include <iostream>

#include "format.hpp"
#include "lowrenderer.hpp"

#include "utils/checkleak.hpp"

#include "application.hpp"

Application::Application()
{
	glfwInit();
	windowInit();

	CHKLK_ENTRY_SNAP
	// init GLFW for Vulkan
	if (!glfwVulkanSupported())
		throw std::exception("GLFW failed to find the Vulkan loader");

	// gather Vulkan extensions required by GLFW
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;

	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> requiredExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	// give the extensions to the low renderer (api instance)
	// create the Vulkan instance first
	rdr.api.initGraphicsAPI(requiredExtensions);

	// create a surface using the instance
	if (glfwCreateWindowSurface(rdr.api.instance, window, nullptr, &rdr.api.surface) != VK_SUCCESS)
		throw std::exception("Failed to create window surface");

	rdr.initRenderer();
}

Application::~Application()
{
	rdr.terminateRenderer();
	CHKLK_EXIT_SNAP

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

#include "vertex.hpp"
#include "vertexbuffer.hpp"

void Application::loop()
{
	// TODO : store vbos in a scene
	Vertex vertices[] = {
		{ { 0.0f, -0.5f}, Color::red },
		{ { 0.5f,  0.5f}, Color::green },
		{ {-0.5f,  0.5f}, Color::blue }
	};
	Vertex vertices2[] = {
		{ { 0.0f,  0.5f}, Color::white },
		{ {-0.5f, -0.5f}, Color::maroon },
		{ { 0.5f, -0.5f}, Color::lime }
	};

	rdr.createVertexBufferObject(3, vertices);
	rdr.createVertexBufferObject(3, vertices2);

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		//draw frame
		rdr.render();
	}
}