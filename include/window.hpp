#pragma once

#include <glad/vulkan.h>
#include <glfw/glfw3.h>

#include "mathematics.hpp"

#include "device.hpp"



class Swapchain
{
private:
	const LogicalDevice& device;

private:
	VkSwapchainKHR handle;


public:
	VkFormat imageFormat;
	VkExtent2D imageExtent;

	std::vector<VkImage> images;
	std::vector<VkImageView> imageViews;


public:
	Swapchain() = delete;
	Swapchain(const VkInstance& instance,
		const VkSurfaceKHR& surface,
		const PhysicalDevice& physicalDevice,
		const LogicalDevice& device)
		: device(device)
	{
		auto support = physicalDevice.querySwapchainSupport(surface);

		VkSurfaceFormatKHR surfaceFormat;
		support.tryFindFormat(VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR, surfaceFormat);
		VkPresentModeKHR presentMode;
		support.tryFindPresentMode(VK_PRESENT_MODE_MAILBOX_KHR, presentMode);
		VkExtent2D extent = support.getExtent();

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


		auto graphicsFamily = physicalDevice.findQueueFamily(VK_QUEUE_GRAPHICS_BIT);
		auto presentFamily = physicalDevice.findPresentFamily(surface);
		uint32_t queueFamilyIndices[] = {
			graphicsFamily.value(),
			presentFamily.value(),
		};


		if (graphicsFamily != presentFamily)
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

		if (vkCreateSwapchainKHR(device.handle, &createInfo, nullptr, &handle) != VK_SUCCESS)
			throw std::runtime_error("Failed to create swapchain");

		vkGetSwapchainImagesKHR(device.handle, handle, &imageCount, nullptr);
		images.resize(imageCount);
		vkGetSwapchainImagesKHR(device.handle, handle, &imageCount, images.data());

		imageFormat = surfaceFormat.format;
		imageExtent = extent;
	}
	~Swapchain()
	{
		for (VkImageView& imageView : imageViews)
		{
			vkDestroyImageView(device.handle, imageView, nullptr);
		}
		vkDestroySwapchainKHR(device.handle, handle, nullptr);
	}
};



class Surface
{
private:
	const VkInstance& instance;


public:
	VkSurfaceKHR handle;

public:
	Surface() = delete;
	Surface(const VkInstance& instance, const VkSurfaceKHR& surface)
		: instance(instance), handle(surface)
	{
	}
	~Surface()
	{
		vkDestroySurfaceKHR(instance, handle, nullptr);
	}
};



class PresentationWindow
{
private:
	std::unique_ptr<Swapchain> swapchain;
	std::unique_ptr<Surface> surface;

public:
	// default extent : HD ready resolution
	uint32_t width = 1366;
	uint32_t height = 768;

	int framebufferWidth;
	int framebufferHeight;

	GLFWwindow* handle;


	PresentationWindow() = delete;
	PresentationWindow(const GraphicsAPIE api, const uint32_t width, const uint32_t height)
		: width(width), height(height)
	{
		glfwInit();

		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		switch (api)
		{
		case GraphicsAPIE::OPENGL:
		{
			glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

			break;
		}
		case GraphicsAPIE::VULKAN:
		{
			// no api specified to create vulkan context
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

			break;
		}
		default:
			throw std::runtime_error("Invalid graphics API");
		}

		handle = glfwCreateWindow(width, height, "Renderer", nullptr, nullptr);
		glfwGetFramebufferSize(handle, &framebufferWidth, &framebufferHeight);
	}
	~PresentationWindow()
	{
		swapchain.reset();
		surface.reset();
		glfwDestroyWindow(handle);
		glfwTerminate();
	}

	const std::vector<const char*> getRequiredExtensions() const
	{
		uint32_t count = 0;
		const char** extensions;

		extensions = glfwGetRequiredInstanceExtensions(&count);

		return std::vector<const char*>(extensions, extensions + count);
	}

	const Surface& createSurface(const VkInstance& instance)
	{
		VkSurfaceKHR surfaceHandle;
		glfwCreateWindowSurface(instance, handle, nullptr, &surfaceHandle);

		surface = std::make_unique<Surface>(instance, surfaceHandle);
		return *surface;
	}

	const Swapchain& createSwapchain(const Context& context)
	{
		swapchain = std::make_unique<Swapchain>(context.instance,
			surface->handle,
			context.deviceSelector->getPhysicalDevice(),
			context.deviceSelector->getLogicalDevice());
		return *swapchain;
	}

	inline void makeContextCurrent()
	{
		glfwMakeContextCurrent(handle);
	}

	inline bool shouldClose() const
	{
		return glfwWindowShouldClose(handle);
	}
};