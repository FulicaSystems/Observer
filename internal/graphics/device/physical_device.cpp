#include "physical_device.hpp"

    PhysicalDevice::PhysicalDevice(const PhysicalDevice& copy)
	{

	}
    PhysicalDevice::PhysicalDevice& operator=(const PhysicalDevice& copy)
	{
		return *this;
	}

	PhysicalDevice::PhysicalDevice(const Context& cx)
		: cx(cx)
	{
	}

	void PhysicalDevice::SelectDevice

	std::unique_ptr<LogicalDevice> PhysicalDevice::createDevice(const VkSurfaceKHR* presentationSurface = nullptr)
	{
		auto graphicsFamily = findQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT);
		auto presentFamily = presentationSurface ? findPresentQueueFamilyIndex(*presentationSurface) :
			std::optional<uint32_t>();
		std::set<uint32_t> uniqueQueueFamilies;
		if (graphicsFamily.has_value())
			uniqueQueueFamilies.insert(graphicsFamily.value());
		if (presentFamily.has_value())
			uniqueQueueFamilies.insert(presentFamily.value());


		float queuePriority = 1.f;
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		for (uint32_t queueFamily : uniqueQueueFamilies)
		{
			VkDeviceQueueCreateInfo queueCreateInfo = {
				.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
				.queueFamilyIndex = queueFamily,
				.queueCount = 1,
				.pQueuePriorities = &queuePriority
			};
			queueCreateInfos.emplace_back(queueCreateInfo);
		}

		VkDeviceCreateInfo createInfo = {
			.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
			.pQueueCreateInfos = queueCreateInfos.data(),
			.enabledLayerCount = Context::layerCount,
			.ppEnabledLayerNames = &Context::layers,
			.enabledExtensionCount = deviceExtensionCount,
			.ppEnabledExtensionNames = &deviceExtensions,
		};

		// create device
		std::unique_ptr<LogicalDevice> out = std::make_unique<LogicalDevice>();
		if (vkCreateDevice(handle, &createInfo, nullptr, &out->handle) != VK_SUCCESS)
			throw std::runtime_error("Failed to create logical device");

		// retrieve queues
		if (graphicsFamily.has_value())
			vkGetDeviceQueue(out->handle,
				graphicsFamily.value(),
				0,
				&out->graphicsQueue);
		if (presentFamily.has_value())
			vkGetDeviceQueue(out->handle,
				presentFamily.value(),
				0,
				&out->presentQueue);
		//if (decodeFamily.has_value())
		//	vkGetDeviceQueue(out->handle,
		//		decodeFamily.value(),
		//		0,
		//		&out->decodeQueue);


		VkCommandPoolCreateInfo resetCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
			.queueFamilyIndex = findQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT).value(),
		};
		if (vkCreateCommandPool(out->handle, &resetCreateInfo, nullptr, &out->commandPool))
			throw std::runtime_error("Failed to create reset command pool");
		VkCommandPoolCreateInfo transientCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
			.queueFamilyIndex = findQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT).value(),
		};
		if (vkCreateCommandPool(out->handle, &transientCreateInfo, nullptr, &out->commandPoolTransient))
			throw std::runtime_error("Failed to create transient command pool");
		//VkCommandPoolCreateInfo decodeCreateInfo = {
		//	.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		//	.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		//	.queueFamilyIndex = findQueueFamilyIndex(VK_QUEUE_VIDEO_DECODE_BIT_KHR).value(),
		//};
		//if (vkCreateCommandPool(out->handle, &resetCreateInfo, nullptr, &out->commandPoolDecode))
		//	throw std::runtime_error("Failed to create reset command pool");

		return out;
	}

	std::optional<uint32_t> PhysicalDevice::findQueueFamilyIndex(const VkQueueFlags& capabilities) const
	{
		for (uint32_t i = 0; i < queueFamilies.size(); ++i)
		{
			if (queueFamilies[i].queueFlags & capabilities)
				return std::optional<uint32_t>(i);
		}
		return std::optional<uint32_t>();
	}
	std::optional<uint32_t> PhysicalDevice::findPresentQueueFamilyIndex(const VkSurfaceKHR& surface) const
	{
		for (uint32_t i = 0; i < queueFamilies.size(); ++i)
		{
			VkBool32 supported;
			vkGetPhysicalDeviceSurfaceSupportKHR(handle, i, surface, &supported);
			if (supported)
				return std::optional<uint32_t>(i);
		}
		return std::optional<uint32_t>();
	}

	class SwapchainSupport PhysicalDevice::querySwapchainSupport(const VkSurfaceKHR& surface) const
	{
		SwapchainSupport details;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(handle, surface, &details.capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(handle, surface, &formatCount, nullptr);
		if (formatCount != 0)
		{
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(handle, surface, &formatCount, details.formats.data());
		}

		uint32_t modeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(handle, surface, &modeCount, nullptr);
		if (modeCount != 0)
		{
			details.presentModes.resize(modeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(handle, surface, &modeCount, details.presentModes.data());
		}

		return details;
	}