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

	// init GLFW for Vulkan
	if (!glfwVulkanSupported())
		throw std::exception("GLFW failed to find the Vulkan loader");

	// gather Vulkan extensions required by GLFW
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;

	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> requiredExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	// give the extensions to the low renderer
	rdr.low.loadExtensions(requiredExtensions);

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

#include "vertex.hpp"
#include "vertexbuffer.hpp"

void Application::loop()
{
	// TODO : store vbos in a scene
	VertexBuffer vbo = VertexBuffer::createBufferObject(rdr.device,
		3,
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	Vertex vertices[] = {
		{ {0.0f, -0.5f}, Color::red },
		{ {0.5f,  0.5f}, Color::green },
		{ {-0.5f, 0.5f}, Color::blue }
	};
	vbo.populate(vertices);
	
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		//draw frame
		rdr.renderScene({ vbo.buffer });
	}

	// TODO : destroy when pipeline is destroyed
	vbo.destroy();
}