#pragma once

#include <glad/vulkan.h>
#include <GLFW/glfw3.h>

class Application
{
	GLFWwindow* window;
	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;

	void windowInit();
	void vulkanInit();
	void vulkanDestroy();
	void vulkanCreate();
	void vulkanDebugMessenger();
	void vulkanExtensions();
	void vulkanLayers();
};