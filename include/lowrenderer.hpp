#pragma once

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

class LowRenderer : public Utils::Singleton<LowRenderer>
{
	friend class Utils::Singleton<LowRenderer>;

private:
	LowRenderer() = default;

	VkDebugUtilsMessengerEXT debugMessenger;

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
		void* userData);

	void vulkanInit();
	void vulkanDestroy();
	void vulkanCreate();

	void vulkanDebugMessenger();
	void vulkanExtensions();
	void vulkanLayers();

	void vulkanSurface(GLFWwindow* window);

public:
	static VkInstance instance;
	static VkSurfaceKHR surface;

	void create(GLFWwindow* window);
	void destroy();
};