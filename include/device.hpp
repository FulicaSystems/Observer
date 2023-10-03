#pragma once

#include <stdexcept>
#include <vector>
#include <iostream>

#include <glad/vulkan.h>

#include "context.hpp"

class PhysicalDevice
{
public:
	VkPhysicalDevice handle;

	std::vector<VkQueueFamilyProperties> queueFamilies;

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
				handle = device;
				//break;
			}
		}

		if (handle == VK_NULL_HANDLE)
			throw std::runtime_error("Failed to find a suitable GPU");

		// physical device limits
		VkPhysicalDeviceProperties prop;
		vkGetPhysicalDeviceProperties(handle, &prop);
		VkPhysicalDeviceLimits limit = prop.limits;
		std::cout << "Physical device max memory allocation count : " << limit.maxMemoryAllocationCount << std::endl;

		// queue families
		uint32_t queueFamilyCount;
		vkGetPhysicalDeviceQueueFamilyProperties(handle, &queueFamilyCount, nullptr);
		queueFamilies.resize(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(handle, &queueFamilyCount, queueFamilies.data());
	}
	~PhysicalDevice() = default;
};

class LogicalDevice
{
public:
	static constexpr const uint32_t deviceExtensionCount = 1;
	static constexpr const const char* deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

public:
	VkDevice handle;
	VkQueue graphicsQueue;
	VkQueue presentQueue;
	VkQueue decodeQueue;

public:
	LogicalDevice() = delete;
	LogicalDevice(const VkInstance& instance,
		const PhysicalDevice& physicalDevice)
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
			.enabledLayerCount = Context::validationLayerCount,
			.ppEnabledLayerNames = &Context::validationLayers,
	#endif
			.enabledExtensionCount = deviceExtensionCount,
			.ppEnabledExtensionNames = &deviceExtensions,
		};

		if (vkCreateDevice(physicalDevice.handle, &createInfo, nullptr, &handle) != VK_SUCCESS)
			throw std::runtime_error("Failed to create logical device");
		if (!gladLoaderLoadVulkan(instance, physicalDevice.handle, handle))
			throw std::runtime_error("Unable to reload Vulkan symbols with logical device");

		vkGetDeviceQueue(handle, indices.graphicsFamily.value(), 0, &graphicsQueue);
		vkGetDeviceQueue(handle, indices.presentFamily.value(), 0, &presentQueue);
	}
	~LogicalDevice()
	{
		vkDestroyDevice(handle, nullptr);
	}
};