#include <set>

#include "context.hpp"
#include "device.hpp"
#include "surface.hpp"

#include "physical_device.hpp"

PhysicalDevice::PhysicalDevice(const Context &cx, const char *deviceName) : cx(cx)
{
    std::optional<VkPhysicalDevice> handle = cx.getPhysicalDeviceHandleByName(deviceName);
    if (!handle.has_value())
        return;

    m_handle = std::make_shared<VkPhysicalDevice>(handle.value());

    memcpy(this->deviceName, deviceName, 256);
    VkPhysicalDeviceProperties props;
    cx.vkGetPhysicalDeviceProperties(*m_handle, &props);
    m_properties = props;
    m_limits = m_properties.limits;

    uint32_t queueFamilyPropertiesCount;
    cx.vkGetPhysicalDeviceQueueFamilyProperties(*m_handle, &queueFamilyPropertiesCount, nullptr);
    m_queueFamilies.resize(queueFamilyPropertiesCount);
    cx.vkGetPhysicalDeviceQueueFamilyProperties(*m_handle, &queueFamilyPropertiesCount, m_queueFamilies.data());
}

std::unique_ptr<LogicalDevice> PhysicalDevice::createDevice(const Surface *presentationSurface)
{
    auto graphicsFamily = findQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT);
    auto presentFamily =
        presentationSurface ? findPresentQueueFamilyIndex(presentationSurface) : std::optional<uint32_t>();
    std::set<uint32_t> uniqueQueueFamilies;
    if (graphicsFamily.has_value())
        uniqueQueueFamilies.insert(graphicsFamily.value());
    if (presentFamily.has_value())
        uniqueQueueFamilies.insert(presentFamily.value());

    float queuePriority = 1.f;
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    for (uint32_t queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo = {.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                                                   .queueFamilyIndex = queueFamily,
                                                   .queueCount = 1,
                                                   .pQueuePriorities = &queuePriority};
        queueCreateInfos.emplace_back(queueCreateInfo);
    }

    VkDeviceCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
        .pQueueCreateInfos = queueCreateInfos.data(),
        .enabledLayerCount = cx.getLayers().size(),
        .ppEnabledLayerNames = cx.getLayers().data(),
        .enabledExtensionCount = deviceExtensionCount,
        .ppEnabledExtensionNames = &deviceExtensions,
    };

    // create device
    std::unique_ptr<LogicalDevice> out = std::make_unique<LogicalDevice>(cx, *this);
    if (cx.vkCreateDevice(*m_handle, &createInfo, nullptr, &out->m_handle) != VK_SUCCESS)
        throw std::runtime_error("Failed to create logical device");
    out->loadDeviceFunctions();

    // retrieve queues
    if (graphicsFamily.has_value())
        out->vkGetDeviceQueue(out->m_handle, graphicsFamily.value(), 0, &out->graphicsQueue);
    if (presentFamily.has_value())
        out->vkGetDeviceQueue(out->m_handle, presentFamily.value(), 0, &out->presentQueue);
    // if (decodeFamily.has_value())
    //	out->vkGetDeviceQueue(out->m_handle,
    //		decodeFamily.value(),
    //		0,
    //		&out->decodeQueue);

    VkCommandPoolCreateInfo resetCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = findQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT).value(),
    };
    if (out->vkCreateCommandPool(out->m_handle, &resetCreateInfo, nullptr, &out->commandPool))
        throw std::runtime_error("Failed to create reset command pool");
    VkCommandPoolCreateInfo transientCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
        .queueFamilyIndex = findQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT).value(),
    };
    if (out->vkCreateCommandPool(out->m_handle, &transientCreateInfo, nullptr, &out->commandPoolTransient))
        throw std::runtime_error("Failed to create transient command pool");
    // VkCommandPoolCreateInfo decodeCreateInfo = {
    //	.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
    //	.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
    //	.queueFamilyIndex = findQueueFamilyIndex(VK_QUEUE_VIDEO_DECODE_BIT_KHR).value(),
    // };
    // if (out->vkCreateCommandPool(out->m_handle, &resetCreateInfo, nullptr, &out->commandPoolDecode))
    //	throw std::runtime_error("Failed to create reset command pool");

    return std::move(out);
}

std::optional<uint32_t> PhysicalDevice::findQueueFamilyIndex(const VkQueueFlags &capabilities) const
{
    for (uint32_t i = 0; i < m_queueFamilies.size(); ++i)
    {
        if (m_queueFamilies[i].queueFlags & capabilities)
            return std::optional<uint32_t>(i);
    }
    return std::optional<uint32_t>();
}
std::optional<uint32_t> PhysicalDevice::findPresentQueueFamilyIndex(const Surface *surface) const
{
    if (!surface)
        return std::optional<uint32_t>();

    for (uint32_t i = 0; i < m_queueFamilies.size(); ++i)
    {
        VkBool32 supported;
        cx.vkGetPhysicalDeviceSurfaceSupportKHR(*m_handle, i, surface->getHandle(), &supported);
        if (supported)
            return std::optional<uint32_t>(i);
    }
    return std::optional<uint32_t>();
}

// class SwapchainSupport PhysicalDevice::querySwapchainSupport(const VkSurfaceKHR& surface) const
//{
//	SwapchainSupport details;

//	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(handle, surface, &details.capabilities);

//	uint32_t formatCount;
//	vkGetPhysicalDeviceSurfaceFormatsKHR(handle, surface, &formatCount, nullptr);
//	if (formatCount != 0)
//	{
//		details.formats.resize(formatCount);
//		vkGetPhysicalDeviceSurfaceFormatsKHR(handle, surface, &formatCount, details.formats.data());
//	}

//	uint32_t modeCount;
//	vkGetPhysicalDeviceSurfacePresentModesKHR(handle, surface, &modeCount, nullptr);
//	if (modeCount != 0)
//	{
//		details.presentModes.resize(modeCount);
//		vkGetPhysicalDeviceSurfacePresentModesKHR(handle, surface, &modeCount, details.presentModes.data());
//	}

//	return details;
//}