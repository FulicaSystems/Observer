#pragma once

#include <memory>
#include <vector>
#include <optional>
#include <deque>

#include <glad/vulkan.h>

#include "utils/derived.hpp"

#include "lowrenderer.hpp"

// comment this macro definition to use a custom memory allocator
#define USE_VMA // define this macro to use Vulkan Memory Allocator

#ifndef NDEBUG
const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};
#endif

const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

using VkQueueFamilyIndex = std::optional<uint32_t>;
struct VkQueueFamilyIndices
{
	//a queue family that supports graphics commands
	VkQueueFamilyIndex graphicsFamily;
	//a queue family that supports presenting images to the surface
	VkQueueFamilyIndex presentFamily;

	bool isComplete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
};
struct VkSwapchainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

/**
 * Physical device (GPU).
 */
class PhysicalDevice
{
public:
	VkPhysicalDevice vkpdevice = VK_NULL_HANDLE;

	PhysicalDevice() = default;
	PhysicalDevice(VkPhysicalDevice vk);

	bool checkDeviceExtensionSupport();
	VkSwapchainSupportDetails querySwapchainSupport(const VkSurfaceKHR& surface);

	/**
	 * Find a queue family capable of VK_QUEUE_GRAPHICS_BIT and presenting images.
	 */
	VkQueueFamilyIndices findQueueFamilies(const VkSurfaceKHR& surface);

	/**
	 * Checks to tell if the device can do the given tasks.
	 */
	bool isDeviceSuitable(const VkSurfaceKHR& surface);

	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;
};

class IAllocation
{
public:
	virtual ~IAllocation() {}
};

struct IMemoryAllocator
{
	virtual void createAllocatorInstance() = 0;
	virtual void destroyAllocatorInstance() = 0;

	IMemoryAllocator() { createAllocatorInstance(); }
	virtual ~IMemoryAllocator() { destroyAllocatorInstance(); }

	virtual void allocateBufferObjectMemory(class VkBufferCreateInfo& createInfo,
		class IVertexBuffer* vbo,
		uint32_t memoryFlags = 0,
		bool mappable = false) = 0;
	virtual void destroyBufferObjectMemory(class IVertexBuffer* vbo) = 0;

	virtual void mapMemory(IAllocation* allocation, void** ppData) = 0;
	virtual void unmapMemory(IAllocation* allocation) = 0;
};


/**
 * Low level rendering instance.
 */
class LowRenderer_Vk : public ILowRenderer
{
	SUPER(ILowRenderer)

private:
	// some extensions are required
	std::vector<const char*> additionalExtensions;

	VkDebugUtilsMessengerEXT debugMessenger;

	struct LogicalDevice;
	std::unique_ptr<LogicalDevice> devicePImpl = nullptr;
	struct GraphicsPipeline;
	std::unique_ptr<GraphicsPipeline> pipelinePImpl = nullptr;
	struct CommandPool;
	std::unique_ptr<CommandPool> commandPoolPImpl = nullptr;
	struct MemoryAllocator;
	std::unique_ptr<MemoryAllocator> allocatorPImpl = nullptr;


	// every created buffer objects
	std::deque<std::shared_ptr<IVertexBuffer>> vbos;


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
	std::shared_ptr<class IShaderModule> createShaderModule_Impl(size_t vsSize,
		size_t fsSize,
		char* vs,
		char* fs) override;
	void destroyShaderModule_Impl(std::shared_ptr<class IShaderModule> ptr) override;

public:
	VkInstance instance;
	// surface must be initialized using the windowing framework
	VkSurfaceKHR surface;

	void initRendererModules();
	void terminateRendererModules();
	void terminateGraphicsAPI();

	// vertex buffer object

	/**
	 * Create a vertex buffer and store it to the vertex buffer container.
	 */
	IVertexBuffer& addVBO(std::shared_ptr<IVertexBuffer> vbo);


	// rendering

	/**
	 * Rendering a single frame.
	 */
	void renderFrame();
};

#ifdef USE_VMA
#include "vmahelper.hpp"

class Alloc_VMA : public IAllocation
{
public:
	VmaAllocation allocation;
};
#else
class Alloc : public IAllocation
{
public:
	// binded memory block
	struct LowRenderer_Vk::MemoryAllocator::MemoryBlock* memoryBlock = nullptr;
	size_t memoryOffset = 0;
};
#endif