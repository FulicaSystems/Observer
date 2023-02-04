#pragma once

#include <optional>
#include <vector>

#include <glad/vulkan.h>

#include "graphicsobject.hpp"

#include "lowrenderer.hpp"

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
private:
	VkPhysicalDevice vkDevice = VK_NULL_HANDLE;

public:
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

	const VkPhysicalDevice& getDevice() const;
};

/**
 * Logical device.
 */
class LogicalDevice : public IGraphicsObject
{
private:
	PhysicalDevice pdevice;

	VkDevice device;

	void vulkanPhysicalDevice(VkInstance instance, VkSurfaceKHR surface);
	void vulkanLogicalDevice(VkInstance instance, VkSurfaceKHR surface);

public:
	LowRenderer& low;

	VkQueue graphicsQueue;
	VkQueue presentQueue;

	LogicalDevice(LowRenderer& low);

	void create() override;
	void destroy() override;

	/**
	 * Get the physical device.
	 */
	const VkPhysicalDevice& getPDevice() const;

	/**
	 * Get the logical device.
	 */
	const VkDevice& getLDevice() const;
};