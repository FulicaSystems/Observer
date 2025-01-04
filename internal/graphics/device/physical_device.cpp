#include <set>

#include "context.hpp"
#include "device.hpp"
#include "surface.hpp"

#include "physical_device.hpp"

void PhysicalDevice::initPhysicalDeviceProperties()
{
    VkPhysicalDeviceProperties props;
    cx.vkGetPhysicalDeviceProperties(*m_handle, &props);
    m_properties = props;
    m_limits = m_properties.limits;
}

void PhysicalDevice::initQueueFamilyProperties()
{
    uint32_t queueFamilyPropertiesCount;
    cx.vkGetPhysicalDeviceQueueFamilyProperties(*m_handle, &queueFamilyPropertiesCount, nullptr);
    m_queueFamilies.resize(queueFamilyPropertiesCount);
    cx.vkGetPhysicalDeviceQueueFamilyProperties(*m_handle, &queueFamilyPropertiesCount, m_queueFamilies.data());
}

void PhysicalDevice::initQueueFamilyIndices(const Surface *presentationSurface)
{
    m_graphicsFamilyIndex = findQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT);
    m_presentFamilyIndex =
        presentationSurface ? findPresentQueueFamilyIndex(presentationSurface) : std::optional<uint32_t>();
#ifdef ENABLE_VIDEO_TRANSCODE
    m_decodeFamilyIndex = findQueueFamilyIndex(VK_QUEUE_VIDEO_DECODE_BIT_KHR);
    m_encodeFamilyIndex = findQueueFamilyIndex(VK_QUEUE_VIDEO_ENCODE_BIT_KHR);
#endif
}

PhysicalDevice::PhysicalDevice(const Context &cx, const char *deviceName) : cx(cx)
{
    std::optional<VkPhysicalDevice> handle = cx.getPhysicalDeviceHandleByName(deviceName);
    if (!handle.has_value())
        return;

    m_handle = std::make_shared<VkPhysicalDevice>(handle.value());

    memcpy(this->deviceName, deviceName, 256);

    initPhysicalDeviceProperties();
    initQueueFamilyProperties();
    initQueueFamilyIndices();

    enumerateAvailableDeviceExtensions();
}

std::vector<std::string> PhysicalDevice::enumerateAvailableDeviceExtensions(const bool bDump) const
{
    uint32_t extensionCount = 0;
    cx.vkEnumerateDeviceExtensionProperties(*m_handle, nullptr, &extensionCount, nullptr);

    std::vector<std::string> out;
    out.reserve(extensionCount);

    std::vector<VkExtensionProperties> extensions(extensionCount);
    cx.vkEnumerateDeviceExtensionProperties(*m_handle, nullptr, &extensionCount, extensions.data());
    if (bDump)
        std::cout << "available device extensions for " << deviceName << " : " << extensionCount << '\n';
    for (const auto &extension : extensions)
    {
        if (bDump)
            std::cout << '\t' << extension.extensionName << '\n';

        out.push_back(extension.extensionName);
    }
    return out;
}

std::unique_ptr<LogicalDevice> PhysicalDevice::createDevice() const
{
    std::set<uint32_t> uniqueQueueFamilies;

    if (m_graphicsFamilyIndex.has_value())
        uniqueQueueFamilies.insert(m_graphicsFamilyIndex.value());
    if (m_presentFamilyIndex.has_value())
        uniqueQueueFamilies.insert(m_presentFamilyIndex.value());
#ifdef ENABLE_VIDEO_TRANSCODE
    if (m_decodeFamilyIndex.has_value())
        uniqueQueueFamilies.insert(m_decodeFamilyIndex.value());
    if (m_encodeFamilyIndex.has_value())
        uniqueQueueFamilies.insert(m_encodeFamilyIndex.value());
#endif

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

    std::vector<const char *> layers = cx.getLayers();
    std::vector<const char *> deviceExtensions = cx.getDeviceExtensions();

    VkDeviceCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
        .pQueueCreateInfos = queueCreateInfos.data(),
        .enabledLayerCount = layers.size(),
        .ppEnabledLayerNames = layers.data(),
        .enabledExtensionCount = deviceExtensions.size(),
        .ppEnabledExtensionNames = deviceExtensions.data(),
    };

    // create device
    std::unique_ptr<LogicalDevice> out = std::make_unique<LogicalDevice>(cx, *this);
    if (cx.vkCreateDevice(*m_handle, &createInfo, nullptr, &out->getHandle()) != VK_SUCCESS)
        throw std::runtime_error("Failed to create logical device");

    out->readyUp();
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

SurfaceDetailsT PhysicalDevice::querySurfaceDetails(const Surface& surface) const
{
	SurfaceDetailsT details;

	cx.vkGetPhysicalDeviceSurfaceCapabilitiesKHR(*m_handle, surface.getHandle(), &details.capabilities);

	uint32_t formatCount;
	cx.vkGetPhysicalDeviceSurfaceFormatsKHR(*m_handle, surface.getHandle(), &formatCount, nullptr);
	if (formatCount != 0)
	{
		details.formats.resize(formatCount);
		cx.vkGetPhysicalDeviceSurfaceFormatsKHR(*m_handle, surface.getHandle(), &formatCount, details.formats.data());
	}

	uint32_t modeCount;
	cx.vkGetPhysicalDeviceSurfacePresentModesKHR(*m_handle, surface.getHandle(), &modeCount, nullptr);
	if (modeCount != 0)
	{
		details.presentModes.resize(modeCount);
		cx.vkGetPhysicalDeviceSurfacePresentModesKHR(*m_handle, surface.getHandle(), &modeCount, details.presentModes.data());
	}

	return details;
}