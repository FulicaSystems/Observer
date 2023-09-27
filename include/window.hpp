#pragma once

#include <glad/vulkan.h>
#include <glfw/glfw3.h>

#include "device.hpp"

class SwapChain
{
private:
	const LogicalDevice& device;

private:
	VkSwapchainKHR handle;

public:
	SwapChain() = delete;
	SwapChain(const VkInstance& instance, const VkSurfaceKHR& surface, const LogicalDevice& device)
		: device(device)
	{
		VkSwapchainSupportDetails support = logicalDevice.pdevice.querySwapchainSupport(surface);

		VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(support.formats);
		VkPresentModeKHR presentMode = chooseSwapPresentMode(support.presentModes);
		VkExtent2D extent = chooseSwapExtent(support.capabilities);

		uint32_t imageCount = support.capabilities.minImageCount + 1;
		if (support.capabilities.maxImageCount > 0 && support.capabilities.maxImageCount < imageCount)
			imageCount = support.capabilities.maxImageCount;

		VkSwapchainCreateInfoKHR createInfo = {
			.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			.surface = surface,
			.minImageCount = imageCount,
			.imageFormat = surfaceFormat.format,
			.imageColorSpace = surfaceFormat.colorSpace,
			.imageExtent = extent,
			.imageArrayLayers = 1,
			.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
			.preTransform = support.capabilities.currentTransform,
			.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			.presentMode = presentMode,
			.clipped = VK_TRUE,
			.oldSwapchain = VK_NULL_HANDLE
		};

		VkQueueFamilyIndices indices = logicalDevice.pdevice.findQueueFamilies(surface);
		uint32_t queueFamilyIndices[] = {
			indices.graphicsFamily.value(),
			indices.presentFamily.value()
		};

		if (indices.graphicsFamily != indices.presentFamily)
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;
		}

		Swapchain swapchain;
		if (vkCreateSwapchainKHR(logicalDevice.vkdevice, &createInfo, nullptr, &swapchain.vkswapchain) != VK_SUCCESS)
			throw std::runtime_error("Failed to create swapchain");

		vkGetSwapchainImagesKHR(logicalDevice.vkdevice, swapchain.vkswapchain, &imageCount, nullptr);
		swapchain.swapchainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(logicalDevice.vkdevice, swapchain.vkswapchain, &imageCount, swapchain.swapchainImages.data());

		swapchain.swapchainImageFormat = surfaceFormat.format;
		swapchain.swapchainExtent = extent;
	}
	~SwapChain()
	{
		for (VkImageView& imageView : pipeline.swapchain.swapchainImageViews)
		{
			vkDestroyImageView(logicalDevice.vkdevice, imageView, nullptr);
		}
		vkDestroySwapchainKHR(logicalDevice.vkdevice, pipeline.swapchain.vkswapchain, nullptr);
	}
};

class Surface
{
private:
	VkSurfaceKHR handle;

public:
	Surface() = delete;
	Surface(const VkInstance& instance, const Window& window)
	{
		glfwCreateWindowSurface(instance, window, nullptr, &surface);
	}
	~Surface()
	{
		vkDestroySurfaceKHR(instance, surface, nullptr);
	}
};

class PresentationWindow
{
private:
	SwapChain swapchain;
	Surface surface;

public:
	PresentationWindow() = delete;
	PresentationWindow()
	{

	}
	~PresentationWindow()
	{

	}
};