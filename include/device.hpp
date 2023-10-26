#pragma once

#include <stdexcept>
#include <vector>
#include <iostream>
#include <optional>
#include <set>

#include <glad/vulkan.h>

#include "utils/singleton.hpp"

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

	~LogicalDevice()
	{
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



	std::unique_ptr<LogicalDevice> createDevice(const VkSurfaceKHR* presentationSurface = nullptr)
	{
		auto graphicsFamily = findQueueFamily(VK_QUEUE_GRAPHICS_BIT);
		auto presentFamily = presentationSurface ? findPresentFamily(*presentationSurface) :
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
	std::optional<uint32_t> findPresentFamily(const VkSurfaceKHR& surface) const
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

	class SwapchainSupport querySwapchainSupport(const VkSurfaceKHR& surface) const;
};




// TODO : make singleton
class DeviceSelector : public Utils::Singleton<DeviceSelector>
{
	SINGLETON(DeviceSelector)

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

	static const PhysicalDevice& getPhysicalDevice()
	{
		auto& i = getInstance();
		return i.physicalDevices[i.selected];
	}
	static const LogicalDevice& getLogicalDevice()
	{
		auto& i = getInstance();
		return *i.logicalDevices[i.selected];
	}
};



// TODO : move to .cpp
#include "window.hpp"
SwapchainSupport PhysicalDevice::querySwapchainSupport(const VkSurfaceKHR& surface) const
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

// TODO : move to .cpp
#include "buffer.hpp"
template<>
std::shared_ptr<Buffer> LogicalDevice::create<Buffer>() const
{
	std::shared_ptr<Buffer> out = std::make_shared<Buffer>();

	VkBufferCreateInfo createInfo = {
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		// TODO : fill this struct with args
	};


	vkCreateBuffer(handle, &createInfo, nullptr, &out->handle);

	return out;
}
template<>
void LogicalDevice::destroy<Buffer>(std::shared_ptr<Buffer>& data) const
{
	vkDestroyBuffer(handle, data->handle, nullptr);
}

#include "shadermodule.hpp"
template<>
std::shared_ptr<ShaderModule> LogicalDevice::create<ShaderModule>() const
{
	// TODO : create shader module
	return std::make_shared<ShaderModule>();
}
template<>
void LogicalDevice::destroy<ShaderModule>(std::shared_ptr<ShaderModule>& pData) const
{
	vkDestroyShaderModule(handle, pData->handle, nullptr);
}