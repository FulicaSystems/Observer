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

	inline bool isComplete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
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
struct PhysicalDevice
{
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

struct LogicalDevice
{
	VkQueue graphicsQueue;
	VkQueue presentQueue;

	PhysicalDevice pdevice;
	VkDevice vkdevice;

	void waitGraphicsQueue();
	void submitCommandToGraphicsQueue(VkSubmitInfo& submitInfo, VkFence fence = VK_NULL_HANDLE);
	void present(VkPresentInfoKHR& presentInfo);
};

struct Swapchain
{
	// swapchain
	VkSwapchainKHR vkswapchain;
	std::vector<VkImage> swapchainImages;
	VkFormat swapchainImageFormat;
	VkExtent2D swapchainExtent;
	std::vector<VkImageView> swapchainImageViews;
};

struct Framebuffers
{
	// TODO : create framebuffer interface FrameBuffer_Vk : public IFrameBuffer
	std::vector<VkFramebuffer> swapchainFramebuffers;
};

struct GraphicsPipeline
{
	// TODO : make the pipeline independant in order to make different pipelines
	std::atomic_flag readyToDraw = ATOMIC_FLAG_INIT;
	std::shared_ptr<class Shader> shader = nullptr;

	Swapchain swapchain;

	// pipeline
	VkRenderPass renderPass;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;

	// framebuffer
	Framebuffers framebuffers;

	// multithreading
	VkSemaphore renderReadySemaphore;
	VkSemaphore renderDoneSemaphore;
	VkFence renderOnceFence;

	void recordImageCommandBuffer(struct CommandBuffer& cb,
		uint32_t imageIndex,
		const std::deque<std::shared_ptr<class IVertexBuffer>>& vbos);
};

struct CommandBuffer
{
	VkCommandBuffer commandBuffer;

	void reset();

	void beginRecord(VkCommandBufferUsageFlags flags = 0);
	void endRecord();
};

struct CommandPool
{
	VkCommandPool commandPool;
	std::deque<CommandBuffer> cbos;

	CommandBuffer& getCmdBufferByIndex(const int index);
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

	LogicalDevice logicalDevice;
	std::shared_ptr<GraphicsPipeline> pipeline;
	CommandPool commandPool;
	std::shared_ptr<class IMemoryAllocator> memoryAllocator;


	// every created buffer objects
	std::deque<std::shared_ptr<IVertexBuffer>> vbos;


	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
		void* userData);

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availableModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);


	void createAPIInstance(const char* appName) override;
	void destroyAPIInstance() override;

	void printAvailableExtensions();
	void printAvailableLayers();

	void createDebugMessenger();


	PhysicalDevice createPhysicalDevice();
	LogicalDevice createLogicalDevice(PhysicalDevice pdevice);
	void destroyLogicalDevice(LogicalDevice logicalDevice);

	Swapchain createSwapchain(LogicalDevice logicalDevice);
	void createSwapchainImageViews(LogicalDevice logicalDevice, Swapchain& swapchain);

	void createGraphicsPipelineAsync(LogicalDevice logicalDevice, GraphicsPipeline* pipeline);
	void createPipelineRenderPass(LogicalDevice logicalDevice, GraphicsPipeline& pipeline);
	void createPipelineFramebuffers(LogicalDevice logicalDevice, GraphicsPipeline& pipeline);
	void createPipelineMultithreadObjects(LogicalDevice logicalDevice, GraphicsPipeline& pipeline);
	void destroyGraphicsPipeline(LogicalDevice logicalDevice, GraphicsPipeline& pipeline);

	CommandPool createCommandPool(LogicalDevice logicalDevice);
	void destroyCommandPool(LogicalDevice logicalDevice, CommandPool& commandPool);

	CommandBuffer createFloatingCommandBuffer();
	void destroyFloatingCommandBuffer(CommandBuffer& cbo);
	CommandBuffer& createCommandBuffer();
	void destroyCommandBuffer(const int index);


	void initGraphicsAPI_Impl(std::span<void*> args) override;
public:
	void terminateGraphicsAPI() override;



private:
	// vertex buffer object
	[[nodiscard]] std::shared_ptr<class IVertexBuffer> createVertexBufferObject_Impl(uint32_t vertexNum,
		bool mappable,
		std::span<uint32_t> additionalArgs = std::span<uint32_t>()) override;
	void populateVertexBufferObject(class IVertexBuffer& vbo, const struct Vertex* vertices) override;
	[[nodiscard]] std::shared_ptr<class IVertexBuffer> createVertexBuffer_Impl(uint32_t vertexNum,
		const struct Vertex* vertices) override;
	void destroyVertexBuffer_Impl(std::shared_ptr<class IVertexBuffer> ptr) override;


	// shader module
	[[nodiscard]] std::shared_ptr<class IShaderModule> createShaderModule_Impl(size_t vsSize,
		size_t fsSize,
		char* vs,
		char* fs) override;
	void destroyShaderModule_Impl(std::shared_ptr<class IShaderModule> ptr) override;


public:
	VkInstance instance;
	// surface must be initialized using the windowing framework
	VkSurfaceKHR surface;


	void initRendererModules() override;
	void terminateRendererModules() override;


	// vertex buffer object

	/**
	 * Create a vertex buffer and store it to the vertex buffer container.
	 */
	IVertexBuffer& addVBO(std::shared_ptr<IVertexBuffer> vbo) override;


	// rendering

	/**
	 * Rendering a single frame.
	 */
	void renderFrame() override;
};