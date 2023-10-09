#pragma once

#include <stdexcept>
#include <vector>
#include <iostream>
#include <optional>
#include <set>

#include <glad/vulkan.h>

#include "context.hpp"

struct LogicalDevice
{
	friend class PhysicalDevice;
	friend class std::unique_ptr<LogicalDevice>;
	friend std::unique_ptr<LogicalDevice> std::make_unique<LogicalDevice>();
	friend class DeviceSelector;

private:
	LogicalDevice() = default;

public:
	VkDevice handle;

	VkQueue graphicsQueue;
	VkQueue presentQueue;
	VkQueue decodeQueue;

	~LogicalDevice()
	{
		vkDestroyDevice(handle, nullptr);
	}
};

class PhysicalDevice
{
	friend class DeviceSelector;

public:
	static constexpr const uint32_t deviceExtensionCount = 1;
	static constexpr const const char* deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

private:
	const VkInstance& instance;

public:
	VkPhysicalDevice handle;

	VkPhysicalDeviceProperties properties;
	VkPhysicalDeviceLimits limit;


	std::vector<VkQueueFamilyProperties> queueFamilies;

public:
	PhysicalDevice() = delete;
	PhysicalDevice(const VkInstance& instance, const VkPhysicalDevice& handle)
		: instance(instance), handle(handle) {}
	PhysicalDevice(const PhysicalDevice& copy)
		: instance(copy.instance), handle(copy.handle), queueFamilies(copy.queueFamilies) {}
	~PhysicalDevice() = default;

	const PhysicalDevice operator=(const PhysicalDevice& copy)
	{
		return PhysicalDevice(copy);
	}



	std::unique_ptr<LogicalDevice> createDevice()
	{
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		auto graphicsFamily = findQueueFamily(VK_QUEUE_GRAPHICS_BIT);
		auto presentFamily = ;
		std::set<uint32_t> uniqueQueueFamilies = {
			graphicsFamily.value(),
			presentFamily.value()
		};

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

		// create device
		std::unique_ptr<LogicalDevice> out = std::make_unique<LogicalDevice>();
		if (vkCreateDevice(handle, &createInfo, nullptr, &out->handle) != VK_SUCCESS)
			throw std::runtime_error("Failed to create logical device");
		if (!gladLoaderLoadVulkan(instance, handle, out->handle))
			throw std::runtime_error("Unable to reload Vulkan symbols with logical device");


		// retrieve queues
		vkGetDeviceQueue(out->handle,
			graphicsFamily.value(),
			0,
			&out->graphicsQueue);
		vkGetDeviceQueue(out->handle,
			presentFamily.value(),
			0,
			&out->presentQueue);
		//vkGetDeviceQueue(out->handle,
		//	decodeFamily.value(),
		//	0,
		//	&out->decodeQueue);

		return out;
	}

	std::optional<uint32_t> findQueueFamily(const VkQueueFlags& capabilities) const
	{
		for (uint32_t i = 0; i < queueFamilies.size(); ++i)
		{
			if (queueFamilies[i].queueFlags & capabilities)
				return std::optional<uint32_t>(i);
		}
		return std::optional<uint32_t>();
	}
};




class DeviceSelector
{
private:
	uint32_t selected = 0;
	std::vector<PhysicalDevice> physicalDevices;
	std::vector<std::unique_ptr<LogicalDevice>> logicalDevices;

public:
	DeviceSelector() = delete;
	DeviceSelector(const VkInstance& instance)
	{
		// enumerate all physical devices
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
		if (deviceCount == 0)
			throw std::runtime_error("Failed to find GPUs with Vulkan support");

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
		std::cout << "available devices : " << deviceCount << '\n';


		// save all physical devices
		physicalDevices.resize(deviceCount);
		for (int i = 0; i < devices.size(); ++i)
		{
			physicalDevices[i] = PhysicalDevice(instance, devices[i]);

			// save queue families
			uint32_t queueFamilyCount;
			vkGetPhysicalDeviceQueueFamilyProperties(physicalDevices[i].handle,
				&queueFamilyCount,
				nullptr);
			physicalDevices[i].queueFamilies.resize(queueFamilyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(physicalDevices[i].handle,
				&queueFamilyCount,
				physicalDevices[i].queueFamilies.data());


			// print device properties
			vkGetPhysicalDeviceProperties(physicalDevices[i].handle, &physicalDevices[i].properties);
			std::cout << '\t' << physicalDevices[i].properties.deviceName << '\n';

			// print physical device limits
			VkPhysicalDeviceLimits limit = physicalDevices[i].properties.limits;
			std::cout << "Physical device max memory allocation count : " << limit.maxMemoryAllocationCount << std::endl;
		}


		// create all logical devices
		devices.resize(deviceCount);
		for (int i = 0; i < physicalDevices.size(); ++i)
		{
			logicalDevices[i] = physicalDevices[i].createDevice();
		}
	}
	~DeviceSelector()
	{
		logicalDevices.clear();
	}
};