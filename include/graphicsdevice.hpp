#pragma once

#include <optional>
#include <vector>

#include <glad/vulkan.h>

#include "graphicsobject.hpp"

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
	VkPhysicalDevice vkpdevice = VK_NULL_HANDLE;

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

	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;

	const VkPhysicalDevice& getDevice() const;
};

/**
 * Logical device.
 */
class LogicalDevice : public IDerived<LogicalDevice, IGraphicsObject>
{
private:
	PhysicalDevice pdevice;

	VkDevice vkdevice;

	VkQueue graphicsQueue;
	VkQueue presentQueue;

	void vulkanPhysicalDevice();
	void vulkanLogicalDevice();

public:
	void create(LowRenderer* api, LogicalDevice* device) override;
	void destroy() override;

	/**
	 * Get the physical device custom class.
	 */
	const PhysicalDevice& getPDevice() const;

	/**
	 * Get the Vulkan physical device.
	 */
	const VkPhysicalDevice& getVkPDevice() const;

	/**
	 * Get the Vulkan logical device.
	 */
	const VkDevice& getVkLDevice() const;

	void waitGraphicsQueue();

	void submitCommandToGraphicsQueue(VkSubmitInfo& submitInfo, VkFence fence = VK_NULL_HANDLE);

	void present(VkPresentInfoKHR& presentInfo);
};