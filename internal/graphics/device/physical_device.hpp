#pragma once

#include <vulkan/vulkan.h>

class Context;

class PhysicalDevice
{
public:
	static constexpr const uint32_t deviceExtensionCount = 1;
	static constexpr const char* deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

private:
	const Context& cx;

private:
	std::shared_ptr<VkPhysicalDevice> m_handle;

	VkPhysicalDeviceProperties properties;
	VkPhysicalDeviceLimits limit;

	std::vector<VkQueueFamilyProperties> queueFamilies;

public:
	PhysicalDevice() = delete;
	PhysicalDevice(const PhysicalDevice& copy) : cx(copy.cx) { }
	PhysicalDevice& operator=(const PhysicalDevice& copy) { return *this; }
	PhysicalDevice(PhysicalDevice&&) = delete;
    PhysicalDevice& operator=(PhysicalDevice&&) = delete;

	PhysicalDevice(const Context& cx) : cx(cx) { }

	//std::unique_ptr<LogicalDevice> createDevice(const VkSurfaceKHR* presentationSurface = nullptr)

	//std::optional<uint32_t> findQueueFamilyIndex(const VkQueueFlags& capabilities) const
	//std::optional<uint32_t> findPresentQueueFamilyIndex(const VkSurfaceKHR& surface) const

	//class SwapchainSupport querySwapchainSupport(const VkSurfaceKHR& surface) const
};