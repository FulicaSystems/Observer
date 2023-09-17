#pragma once

#include <glad/vulkan.h>

class PhysicalDevice
{
private:
	VkPhysicalDevice physicalDevice;

public:
	PhysicalDevice() = delete;
	PhysicalDevice(const VkInstance& instance)
	{
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
		if (deviceCount == 0)
			throw std::runtime_error("Failed to find GPUs with Vulkan support");

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
		std::cout << "available devices : " << deviceCount << '\n';

		for (const auto& device : devices)
		{
			VkPhysicalDeviceProperties properties;
			vkGetPhysicalDeviceProperties(device, &properties);
			std::cout << '\t' << properties.deviceName << '\n';
		}

		// select the last available device (most likely discrete gpu)
		for (const auto& device : devices)
		{
			PhysicalDevice d(device);
			if (d.isDeviceSuitable(surface))
			{
				pdevice = device;
				//break;
			}
		}

		VkPhysicalDevice d = pdevice.vkpdevice;
		if (d == VK_NULL_HANDLE)
			throw std::runtime_error("Failed to find a suitable GPU");

		// physical device limits
		VkPhysicalDeviceProperties prop;
		vkGetPhysicalDeviceProperties(pdevice.vkpdevice, &prop);
		VkPhysicalDeviceLimits limit = prop.limits;
		std::cout << "Physical device max memory allocation count : " << limit.maxMemoryAllocationCount << std::endl;
	}
	~PhysicalDevice() = default;
};

class LogicalDevice
{
private:
	VkDevice device;

public:
	LogicalDevice() = delete;
	LogicalDevice(const VkInstance& instance)
	{
		VkQueueFamilyIndices indices = pdevice.findQueueFamilies(surface);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

		float queuePriority = 1.f;
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
	#ifdef NDEBUG
			.enabledLayerCount = 0,
	#else
			.enabledLayerCount = static_cast<uint32_t>(validationLayers.size()),
			.ppEnabledLayerNames = validationLayers.data(),
	#endif
			.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()),
			.ppEnabledExtensionNames = deviceExtensions.data()
		};

		VkPhysicalDevice d = pdevice.vkpdevice;

		LogicalDevice logicalDevice;
		logicalDevice.pdevice = pdevice;
		if (vkCreateDevice(d, &createInfo, nullptr, &logicalDevice.vkdevice) != VK_SUCCESS)
			throw std::runtime_error("Failed to create logical device");
		if (!gladLoaderLoadVulkan(instance, d, logicalDevice.vkdevice))
			throw std::runtime_error("Unable to reload Vulkan symbols with logical device");

		vkGetDeviceQueue(logicalDevice.vkdevice, indices.graphicsFamily.value(), 0, &logicalDevice.graphicsQueue);
		vkGetDeviceQueue(logicalDevice.vkdevice, indices.presentFamily.value(), 0, &logicalDevice.presentQueue);
	}
	~LogicalDevice()
	{
		vkDestroyDevice(device, nullptr);
	}
};