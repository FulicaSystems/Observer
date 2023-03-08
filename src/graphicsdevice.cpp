#include <vector>
#include <string>
#include <set>
#include <iostream>

#include "lowrenderer.hpp"
#include "graphicsdevice.hpp"

const VkPhysicalDevice& PhysicalDevice::getDevice() const
{
	return vkpdevice;
}

void LogicalDevice::create(LowRenderer* api, LogicalDevice* device)
{
	Super::create(api, nullptr);

	vulkanPhysicalDevice();
	vulkanLogicalDevice();
}

void LogicalDevice::destroy()
{
	vkDestroyDevice(vkdevice, nullptr);
}

const PhysicalDevice& LogicalDevice::getPDevice() const
{
	return pdevice;
}

const VkPhysicalDevice& LogicalDevice::getVkPDevice() const
{
	return pdevice.getDevice();
}

const VkDevice& LogicalDevice::getVkLDevice() const
{
	return vkdevice;
}

void LogicalDevice::waitGraphicsQueue()
{
	vkQueueWaitIdle(graphicsQueue);
}

void LogicalDevice::submitCommandToGraphicsQueue(VkSubmitInfo& submitInfo, VkFence fence)
{
	if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, fence) != VK_SUCCESS)
		throw std::exception("Failed to submit draw command buffer");
}

void LogicalDevice::present(VkPresentInfoKHR& presentInfo)
{
	vkQueuePresentKHR(presentQueue, &presentInfo);
}

bool PhysicalDevice::checkDeviceExtensionSupport()
{
	uint32_t extensionCount = 0;
	vkEnumerateDeviceExtensionProperties(vkpdevice, nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(vkpdevice, nullptr, &extensionCount, extensions.data());
	std::cout << "available device extensions : " << extensionCount << '\n';
	for (const auto& extension : extensions)
		std::cout << '\t' << extension.extensionName << '\n';

	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for (const VkExtensionProperties& extension : extensions)
	{
		//is the required extension available?
		requiredExtensions.erase(extension.extensionName);
	}

	//are all required extensions found in the available extension list?
	return requiredExtensions.empty();
}

PhysicalDevice::PhysicalDevice(VkPhysicalDevice vk)
{
	vkpdevice = vk;
}

VkSwapchainSupportDetails PhysicalDevice::querySwapchainSupport(const VkSurfaceKHR& surface)
{
	VkSwapchainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkpdevice, surface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(vkpdevice, surface, &formatCount, nullptr);
	if (formatCount != 0)
	{
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(vkpdevice, surface, &formatCount, details.formats.data());
	}

	uint32_t modeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(vkpdevice, surface, &modeCount, nullptr);
	if (modeCount != 0)
	{
		details.presentModes.resize(modeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(vkpdevice, surface, &modeCount, details.presentModes.data());
	}

	return details;
}

bool PhysicalDevice::isDeviceSuitable(const VkSurfaceKHR& surface)
{
#if false
	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceProperties(pdevice, &deviceProperties);
	vkGetPhysicalDeviceFeatures(pdevice, &deviceFeatures);

	return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
		deviceFeatures.geometryShader;
#else
	VkQueueFamilyIndices indices = findQueueFamilies(surface);

	bool extensionSupport = checkDeviceExtensionSupport();

	bool swapchainSupport = false;
	if (extensionSupport)
	{
		VkSwapchainSupportDetails support = querySwapchainSupport(surface);
		swapchainSupport = !support.formats.empty() && !support.presentModes.empty();
	}

	return indices.isComplete() && extensionSupport && swapchainSupport;
#endif
}

uint32_t PhysicalDevice::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const
{
	VkPhysicalDeviceMemoryProperties memProp;
	vkGetPhysicalDeviceMemoryProperties(vkpdevice, &memProp);

	for (uint32_t i = 0; i < memProp.memoryTypeCount; ++i)
	{
		bool rightType = typeFilter & (1 << i);
		bool rightFlag = (memProp.memoryTypes[i].propertyFlags & properties) == properties;
		if (rightType && rightFlag)
			return i;
	}

	throw std::exception("Failed to find suitable memory type");
}

void LogicalDevice::vulkanPhysicalDevice()
{
	VkInstance instance = api->instance;
	VkSurfaceKHR surface = api->surface;

	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
	if (deviceCount == 0)
		throw std::exception("Failed to find GPUs with Vulkan support");

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
	std::cout << "available devices : " << deviceCount << '\n';

	for (const auto& device : devices)
	{
		VkPhysicalDeviceProperties properties;
		vkGetPhysicalDeviceProperties(device, &properties);
		std::cout << '\t' << properties.deviceName << '\n';
	}

	//select the last available device (most likely discrete gpu)
	for (const auto& device : devices)
	{
		PhysicalDevice d(device);
		if (d.isDeviceSuitable(surface))
		{
			pdevice = device;
			//break;
		}
	}

	VkPhysicalDevice d = pdevice.getDevice();
	if (d == VK_NULL_HANDLE)
		throw std::exception("Failed to find a suitable GPU");
	if (!gladLoaderLoadVulkan(instance, d, nullptr))
		throw std::exception("Unable to reload Vulkan symbols with physical device");

	// physical device limits
	VkPhysicalDeviceProperties prop;
	vkGetPhysicalDeviceProperties(pdevice.getDevice(), &prop);
	VkPhysicalDeviceLimits limit = prop.limits;
	std::cout << "Physical device max memory allocation count : " << limit.maxMemoryAllocationCount << std::endl;
}

VkQueueFamilyIndices PhysicalDevice::findQueueFamilies(const VkSurfaceKHR& surface)
{
	VkQueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(vkpdevice, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(vkpdevice, &queueFamilyCount, queueFamilies.data());
	for (uint32_t i = 0; i < queueFamilies.size(); ++i)
	{
		//graphics family
		if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			indices.graphicsFamily = i;

		//presentation family
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(vkpdevice, i, surface, &presentSupport);
		if (presentSupport)
			indices.presentFamily = i;
	}

	return indices;
}

void LogicalDevice::vulkanLogicalDevice()
{
	VkInstance instance = api->instance;
	VkSurfaceKHR surface = api->surface;

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
		queueCreateInfos.push_back(queueCreateInfo);
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

	VkPhysicalDevice d = pdevice.getDevice();

	if (vkCreateDevice(d, &createInfo, nullptr, &vkdevice) != VK_SUCCESS)
		throw std::exception("Failed to create logical device");
	if (!gladLoaderLoadVulkan(instance, d, vkdevice))
		throw std::exception("Unable to reload Vulkan symbols with logical device");

	vkGetDeviceQueue(vkdevice, indices.graphicsFamily.value(), 0, &graphicsQueue);
	vkGetDeviceQueue(vkdevice, indices.presentFamily.value(), 0, &presentQueue);
}