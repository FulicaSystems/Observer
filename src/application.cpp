#include <exception>
#include <vector>
#include <iostream>
#include <stdexcept>

#include "utils/multithread/globalthreadpool.hpp"

#include "format.hpp"
#include "lowrenderer_gl.hpp"
#include "lowrenderer_vk.hpp"

#include <GLFW/glfw3.h>

#include "application.hpp"

Application::Application(const EGraphicsAPI graphicsApi)
{
	windowInit(graphicsApi);

	switch (graphicsApi)
	{
	case EGraphicsAPI::OPENGL:
	{
		glfwMakeContextCurrent(window);

		rdr = new LowRenderer_Gl(graphicsApi);
		rdr->initGraphicsAPI(glfwGetProcAddress);

		break;
	}
	case EGraphicsAPI::VULKAN:
	{
		// init GLFW for Vulkan
		if (!glfwVulkanSupported())
			throw std::runtime_error("GLFW failed to find the Vulkan loader");

		// gather Vulkan extensions required by GLFW
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;

		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> requiredExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		rdr = new LowRenderer_Vk(graphicsApi);
		// give the extensions to the low renderer (api instance)
		// create the Vulkan instance first
		rdr->initGraphicsAPI(&requiredExtensions);

		LowRenderer_Vk* api = (LowRenderer_Vk*)rdr;
		// create a surface using the instance
		if (glfwCreateWindowSurface(api->instance, window, nullptr, &api->surface) != VK_SUCCESS)
			throw std::runtime_error("Failed to create window surface");

		break;
	}
	default:
		throw std::runtime_error("Invalid specified graphics API");
	}

	rdr->initRendererModules();
}

Application::~Application()
{
	((LowRenderer_Vk*)rdr)->terminateGraphicsAPI();

	glfwDestroyWindow(window);
	glfwTerminate();
}

void Application::windowInit(EGraphicsAPI graphicsApi)
{
	glfwInit();

	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	switch (graphicsApi)
	{
	case EGraphicsAPI::OPENGL:
	{
		glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		break;
	}
	case EGraphicsAPI::VULKAN:
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

#include "resourcesmanager.hpp"
#include "mesh.hpp"
#include "shader.hpp"

void Application::loop()
{
	// TODO : store vbos in a scene

	ResourcesManager::load<Mesh>("triangle", "", *rdr);
	ResourcesManager::load<Mesh>("triangle", "", *rdr);
	ResourcesManager::load<Mesh>("triangle", "", *rdr);

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		Utils::GlobalThreadPool::pollMainQueue();

		//draw frame
		((LowRenderer_Vk*)rdr)->renderFrame();
	}
}