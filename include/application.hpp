#pragma once

#include <vector>

#include <glad/vulkan.h>
#include <glfw/glfw3.h>

#include "graphicsapi.hpp"

#include "window.hpp"

#include "context.hpp"
#include "device.hpp"
#include "renderer.hpp"


class Application
{
private:
	std::unique_ptr<PresentationWindow> window;

	std::unique_ptr<Context> context;

	std::unique_ptr<RendererABC> renderer;

public:
	Application(const GraphicsAPIE api)
	{
		window = std::make_unique<PresentationWindow>(api, 1366, 768);

		switch (api)
		{
		case GraphicsAPIE::OPENGL:
		{
			window->makeContextCurrent();
			throw std::runtime_error("not yet implemented");

			break;
		}
		case GraphicsAPIE::VULKAN:
		{
			// init GLFW for Vulkan
			if (!glfwVulkanSupported())
				throw std::runtime_error("GLFW failed to find the Vulkan loader");

			// TODO : clean symbols loading
			gladLoaderLoadVulkan(nullptr, nullptr, nullptr);
			context = std::make_unique<Context>("Renderer",
				VERSION(0, 0, 0),
				VERSION(0, 0, 0),
				window->getRequiredExtensions());
			gladLoaderLoadVulkan(context->instance, nullptr, nullptr);
			context->deviceSelector = std::make_unique<DeviceSelector>(context->instance,
				[this](const VkPhysicalDevice& physicalDevice) {
					gladLoaderLoadVulkan(context->instance,
						physicalDevice,
						nullptr);
				});
			gladLoaderLoadVulkan(context->instance,
				context->deviceSelector->getPhysicalDevice().handle,
				context->deviceSelector->getLogicalDevice().handle);


			// create a surface using the instance
			window->createSurface(context->instance);

			// create swapchain
			auto& swapchain = window->createSwapchain(*context);


			// renderer
			renderer = std::make_unique<ForwardRenderer>(context->deviceSelector->getLogicalDevice(),
				swapchain);

			break;
		}
		default:
			throw std::runtime_error("Invalid specified graphics API");
		}
	}
	~Application()
	{
		renderer.reset();
		window.reset();
		context.reset();
	}


	void run();
};