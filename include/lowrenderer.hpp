#pragma once

#include <vector>

#include <glad/vulkan.h>

#include "graphicsobject.hpp"

#ifndef NDEBUG
const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};
#endif

const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

class LogicalDevice;

/**
 * Low level rendering instance.
 */
class LowRenderer : public IDerived<LowRenderer, IGraphicsObject>
{
private:
	// some extensions are required
	std::vector<const char*> additionalExtensions;

	VkDebugUtilsMessengerEXT debugMessenger;

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
		void* userData);

	void vulkanCreate();
	void vulkanDestroy();

	void vulkanExtensions();
	void vulkanLayers();

	void vulkanDebugMessenger();

public:
	VkInstance instance;
	// surface must be initialized using the windowing framework
	VkSurfaceKHR surface;

	void loadExtensions(std::vector<const char*>& additionalExtensions);

	void create(LowRenderer* api, LogicalDevice* device) override;
	void destroy() override;
};