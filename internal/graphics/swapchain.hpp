#pragma once


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


		auto graphicsFamily = physicalDevice.findQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT);
		auto presentFamily = physicalDevice.findPresentQueueFamilyIndex(surface);
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
