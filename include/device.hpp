#pragma once

#include <stdexcept>
#include <vector>
#include <iostream>
#include <optional>
#include <set>
#include <functional>

#include <glad/vulkan.h>

#include "mathematics.hpp"

#include "context.hpp"



class LogicalDevice
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

	// reset command pool
	VkCommandPool commandPool;
	// transient command pool
	VkCommandPool commandPoolTransient;
	// decode reset command pool
	VkCommandPool commandPoolDecode;



	~LogicalDevice()
	{
		//vkDestroyCommandPool(handle, commandPoolDecode, nullptr);
		vkDestroyCommandPool(handle, commandPoolTransient, nullptr);
		vkDestroyCommandPool(handle, commandPool, nullptr);
		vkDestroyDevice(handle, nullptr);
	}

	template<class TDataType>
	std::shared_ptr<TDataType> create() const { throw std::runtime_error("Use template specialization"); }
	template<class TDataType>
	void destroy(std::shared_ptr<TDataType>& pData) const { throw std::runtime_error("Use template specialization"); }

	template<>
	std::shared_ptr<class Buffer> create<class Buffer>() const;
	template<>
	void destroy<class Buffer>(std::shared_ptr<class Buffer>& pData) const;

	template<>
	std::shared_ptr<class ShaderModule> create<class ShaderModule>() const;
	template<>
	void destroy<class ShaderModule>(std::shared_ptr<class ShaderModule>& pData) const;
};



struct SwapchainSupport
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;

	bool tryFindFormat(const VkFormat& targetFormat,
		const VkColorSpaceKHR& targetColorSpace,
		VkSurfaceFormatKHR& found)
	{
		for (const auto& format : formats)
		{
			if (format.format == targetFormat && format.colorSpace == targetColorSpace)
			{
				found = format;
				return true;
			}
		}

		return false;
	}

	bool tryFindPresentMode(const VkPresentModeKHR& targetPresentMode,
		VkPresentModeKHR& found)
	{
		for (const auto& presentMode : presentModes)
		{
			if (presentMode == targetPresentMode)
			{
				found = presentMode;
				return true;
			}
		}

		return false;
	}

	VkExtent2D getExtent()
	{
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			return capabilities.currentExtent;
		}
		else
		{
			// arbitrary values (HD ready)
			return VkExtent2D{
				.width = Math::clamp(1366U,
					capabilities.minImageExtent.width,
					capabilities.maxImageExtent.width),
				.height = Math::clamp(768U,
					capabilities.minImageExtent.height,
					capabilities.maxImageExtent.height),
			};
		}
	}
};

class PhysicalDevice
{
	friend class DeviceSelector;

public:
	static constexpr const uint32_t deviceExtensionCount = 1;
	static constexpr const char* deviceExtensions = {
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



	std::unique_ptr<LogicalDevice> createDevice(const VkSurfaceKHR* presentationSurface = nullptr)
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

	std::optional<uint32_t> findQueueFamilyIndex(const VkQueueFlags& capabilities) const
	{
		for (uint32_t i = 0; i < queueFamilies.size(); ++i)
		{
			if (queueFamilies[i].queueFlags & capabilities)
				return std::optional<uint32_t>(i);
		}
		return std::optional<uint32_t>();
	}
	std::optional<uint32_t> findPresentQueueFamilyIndex(const VkSurfaceKHR& surface) const
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

	class SwapchainSupport querySwapchainSupport(const VkSurfaceKHR& surface) const
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
};




class DeviceSelector
{
private:
	uint32_t selected = 0;
	std::vector<PhysicalDevice> physicalDevices;
	std::vector<std::unique_ptr<LogicalDevice>> logicalDevices;

public:
	DeviceSelector() = delete;
	DeviceSelector(const VkInstance& instance,
		const std::function<void(const VkPhysicalDevice& physicalDevice)>& physicalDeviceSymbolsLoader)
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
		for (int i = 0; i < devices.size(); ++i)
		{
			physicalDevices.push_back(PhysicalDevice(instance, devices[i]));

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
			std::cout << "\t\tPhysical device max memory allocation count : " << limit.maxMemoryAllocationCount << std::endl;
		}

		if (physicalDeviceSymbolsLoader)
			physicalDeviceSymbolsLoader(getPhysicalDevice().handle);

		// create all logical devices
		logicalDevices.resize(deviceCount);
		for (int i = 0; i < physicalDevices.size(); ++i)
		{
			logicalDevices[i] = physicalDevices[i].createDevice();
		}
	}
	~DeviceSelector()
	{
		logicalDevices.clear();
	}

	const PhysicalDevice& getPhysicalDevice() const
	{
		return physicalDevices[selected];
	}
	const LogicalDevice& getLogicalDevice() const
	{
		return *logicalDevices[selected];
	}
};