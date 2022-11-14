#pragma once

#include <optional>
#include <glad/vulkan.h>

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

class PhysicalDevice
{
private:
	VkPhysicalDevice vkDevice = VK_NULL_HANDLE;

public:
	PhysicalDevice() = default;
	PhysicalDevice(VkPhysicalDevice vk);

	bool checkDeviceExtensionSupport();
	VkSwapchainSupportDetails querySwapchainSupport();

	/**
	 * Find a queue family capable of VK_QUEUE_GRAPHICS_BIT and presenting images.
	 */
	VkQueueFamilyIndices findQueueFamilies();

	/**
	 * Checks to tell if the device can do the given tasks.
	 */
	bool isDeviceSuitable();

	const VkPhysicalDevice& getDevice() const;
};

/**
 * Logical device.
 */
class GraphicsDevice
{
private:
	PhysicalDevice pdevice;

	VkDevice device;

	void vulkanPhysicalDevice(VkInstance instance);
	void vulkanLogicalDevice(VkInstance instance);

public:
	VkQueue graphicsQueue;
	VkQueue presentQueue;

	void create();
	/**
	 * Destroy the logical device.
	 */
	void destroy();

	/**
	 * Get the physical device.
	 */
	const VkPhysicalDevice& getPDevice() const;

	/**
	 * Get the logical device.
	 */
	const VkDevice& getLDevice() const;
};