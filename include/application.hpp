#pragma once

#include <vector>

#include <glad/vulkan.h>
#include <GLFW/glfw3.h>

#include "utils/singleton.hpp"

#include "graphicsdevice.hpp"
#include "graphicspipeline.hpp"

#ifndef NDEBUG
const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};
#endif

const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

class Application : public Utils::Singleton<Application>
{
	friend class Singleton<Application>;

private:
	Application();

	GLFWwindow* window;

	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;

	VkSurfaceKHR surface;

	GraphicsDevice device;
	GraphicsPipeline pipeline;

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
		void* userData);

	void windowInit();

	void vulkanInit();
	void vulkanDestroy();
	void vulkanCreate();

	void vulkanDebugMessenger();
	void vulkanExtensions();
	void vulkanLayers();

	void vulkanSurface();

public:
	void create();
	void destroy();

	void getWindowSize(int& width, int& height);

	const VkSurfaceKHR& getSurface() const;

	const VkInstance& getVkInstance() const;

	void loop();
};