#pragma once

#include <memory>
#include <vector>

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


	// buffer object

	[[nodiscard]] std::shared_ptr<class IVertexBuffer> createBufferObject_Impl(uint32_t vertexNum,
		bool mappable,
		std::span<uint32_t> additionalArgs = std::span<uint32_t>()) override;
	void populateBufferObject(class IVertexBuffer& vbo, const struct Vertex* vertices) override;
public:
	void destroyBufferObject(class IVertexBuffer& vbo) override;


private:
	// vertex buffer object
	std::shared_ptr<class IVertexBuffer> createVertexBuffer_Impl(uint32_t vertexNum,
		const struct Vertex* vertices) override;

	// shader module
	std::shared_ptr<class IShaderModule> createShaderModule_Impl(class ILogicalDevice* device,
		size_t vsSize,
		size_t fsSize,
		char* vs,
		char* fs) override;
	void destroyShaderModule_Impl(std::shared_ptr<class IShaderModule> ptr) override;

	// logical device
	std::shared_ptr<class ILogicalDevice> createLogicalDevice_Impl() override;
	void destroyLogicalDevice_Impl(std::shared_ptr<class ILogicalDevice> ptr) override;

public:
	VkInstance instance;
	// surface must be initialized using the windowing framework
	VkSurfaceKHR surface;

	void terminateGraphicsAPI();
};