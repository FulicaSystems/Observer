#include <exception>
#include <vector>
#include <iostream>

#include "lowrenderer.hpp"

#include "application.hpp"

Application::Application()
{
	glfwInit();
	windowInit();
}

void Application::destroy()
{
	glfwDestroyWindow(window);
	glfwTerminate();
}

void Application::getWindowSize(int& width, int& height)
{
	glfwGetFramebufferSize(window, &width, &height);
}

void Application::windowInit()
{
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(800, 600, "Vulkan window", nullptr, nullptr);
}

void Application::loop()
{
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		//draw frame
	}
}