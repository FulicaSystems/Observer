#pragma once

#include <vector>

#include <glfw/glfw3.h>

#include "graphicsapi.hpp"
#include "format.hpp"


class Application
{
private:
	GLFWwindow* window;

public:
	Application(const GraphicsAPI_E api)
	{
		initWindow(api);

		switch (api)
		{
		case GraphicsAPI_E::OPENGL:
		{
			glfwMakeContextCurrent(window);
			throw std::runtime_error("not yet implemented");

			break;
		}
		case GraphicsAPI_E::VULKAN:
		{
			// init GLFW for Vulkan
			if (!glfwVulkanSupported())
				throw std::runtime_error("GLFW failed to find the Vulkan loader");

			// gather Vulkan extensions required by GLFW
			uint32_t glfwExtensionCount = 0;
			const char** glfwExtensions;

			glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

			std::vector<const char*> requiredExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

			// TODO : create api structs


			// create a surface using the instance
			//if (glfwCreateWindowSurface(api->instance, window, nullptr, &api->surface) != VK_SUCCESS)
			//	throw std::runtime_error("Failed to create window surface");

			break;
		}
		default:
			throw std::runtime_error("Invalid specified graphics API");
		}
	}
	~Application()
	{
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	inline void initWindow(const GraphicsAPI_E api)
	{
		glfwInit();

		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		switch (api)
		{
		case GraphicsAPI_E::OPENGL:
		{
			glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

			break;
		}
		case GraphicsAPI_E::VULKAN:
		{
			// no api specified to create vulkan context
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

			break;
		}
		default:
			throw std::runtime_error("Invalid graphics API");
		}

		window = glfwCreateWindow(Format::height, Format::width, "Renderer", nullptr, nullptr);
		glfwGetFramebufferSize(window, &Format::framebufferHeight, &Format::framebufferWidth);
	}

	void run();
};