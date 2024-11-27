#pragma once

#include <vulkan/vulkan.h>

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
    PhysicalDevice(const PhysicalDevice& copy);
    PhysicalDevice& operator=(const PhysicalDevice& copy);
    PhysicalDevice(PhysicalDevice&&) = delete;
    PhysicalDevice& operator=(PhysicalDevice&&) = delete;

	PhysicalDevice(const Context& cx);

	std::unique_ptr<LogicalDevice> createDevice(const VkSurfaceKHR* presentationSurface = nullptr)

	std::optional<uint32_t> findQueueFamilyIndex(const VkQueueFlags& capabilities) const
	std::optional<uint32_t> findPresentQueueFamilyIndex(const VkSurfaceKHR& surface) const

	class SwapchainSupport querySwapchainSupport(const VkSurfaceKHR& surface) const
};