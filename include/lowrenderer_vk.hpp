#pragma once

#include <vector>
#include <memory>

#include <glad/vulkan.h>

#include "lowrenderer.hpp"

#ifndef NDEBUG
const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};
#endif

const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

/**
 * Low level rendering instance.
 */
class LowRenderer_Vk : public ILowRenderer
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

	void initGraphicsAPI_Impl(std::span<void*> args) override;


	// vertex buffer object

	std::shared_ptr<class VertexBuffer> createVertexBuffer_Impl(uint32_t vertexNum,
		const class Vertex* vertices) override;

	[[nodiscard]] std::shared_ptr<class VertexBuffer> createBufferObject_Impl(uint32_t vertexNum,
		bool mappable,
		std::span<uint32_t> additionalArgs = std::span<uint32_t>()) override;
	void populateBufferObject(class VertexBuffer& vbo, const class Vertex* vertices) override;
public:
	void destroyBufferObject(class VertexBuffer& vbo) override;

public:
	VkInstance instance;
	// surface must be initialized using the windowing framework
	VkSurfaceKHR surface;

	void terminateGraphicsAPI();
};