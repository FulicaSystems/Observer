#include "context.hpp"
#include "physical_device.hpp"

#include "device.hpp"

LogicalDevice::LogicalDevice(const Context &cx, const PhysicalDevice &physicalDevice)
    : cx(cx), physicalHandle(physicalDevice)
{
}

LogicalDevice::~LogicalDevice()
{
#ifdef ENABLE_VIDEO_TRANSCODE
    if (commandPoolEncode)
        vkDestroyCommandPool(m_handle, commandPoolEncode, nullptr);
    if (commandPoolDecode)
        vkDestroyCommandPool(m_handle, commandPoolDecode, nullptr);
#endif

    vkDestroyCommandPool(m_handle, commandPoolTransient, nullptr);
    vkDestroyCommandPool(m_handle, commandPool, nullptr);

    vkDestroyDevice(m_handle, nullptr);
}

void LogicalDevice::readyUp()
{
    loadDeviceFunctions();

    retrieveQueues();
    createCommandPools();
}

void LogicalDevice::loadDeviceFunctions()
{
    VK_GET_DEVICE_PROC_ADDR(m_handle, vkGetDeviceQueue);
    VK_GET_DEVICE_PROC_ADDR(m_handle, vkDestroyDevice);

    VK_GET_DEVICE_PROC_ADDR(m_handle, vkCreateCommandPool);
    VK_GET_DEVICE_PROC_ADDR(m_handle, vkDestroyCommandPool);
}

void LogicalDevice::retrieveQueues()
{
    auto graphicsFamilyIndex = physicalHandle.getGraphicsFamilyIndex();
    auto presentFamilyIndex = physicalHandle.getPresentFamilyIndex();
#ifdef ENABLE_VIDEO_TRANSCODE
    auto decodeFamilyIndex = physicalHandle.getDecodeFamilyIndex();
    auto encodeFamilyIndex = physicalHandle.getEncodeFamilyIndex();
#endif

    if (graphicsFamilyIndex.has_value())
        vkGetDeviceQueue(m_handle, graphicsFamilyIndex.value(), 0, &graphicsQueue);
    if (presentFamilyIndex.has_value())
        vkGetDeviceQueue(m_handle, presentFamilyIndex.value(), 0, &presentQueue);
#ifdef ENABLE_VIDEO_TRANSCODE
    if (decodeFamilyIndex.has_value())
        vkGetDeviceQueue(m_handle, decodeFamilyIndex.value(), 0, &decodeQueue);
    if (encodeFamilyIndex.has_value())
        vkGetDeviceQueue(m_handle, encodeFamilyIndex.value(), 0, &encodeQueue);
#endif
}

void LogicalDevice::createCommandPools()
{
    VkCommandPoolCreateInfo resetCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = physicalHandle.findQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT).value(),
    };

    if (graphicsQueue)
    {
        if (vkCreateCommandPool(m_handle, &resetCreateInfo, nullptr, &commandPool))
            throw std::runtime_error("Failed to create reset command pool");
        VkCommandPoolCreateInfo transientCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
            .queueFamilyIndex = physicalHandle.findQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT).value(),
        };
        if (vkCreateCommandPool(m_handle, &transientCreateInfo, nullptr, &commandPoolTransient))
            throw std::runtime_error("Failed to create transient command pool");
    }

#ifdef ENABLE_VIDEO_TRANSCODE
    if (decodeQueue)
    {
        VkCommandPoolCreateInfo decodeCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .queueFamilyIndex = physicalHandle.findQueueFamilyIndex(VK_QUEUE_VIDEO_DECODE_BIT_KHR).value(),
        };
        if (vkCreateCommandPool(m_handle, &resetCreateInfo, nullptr, &commandPoolDecode))
            throw std::runtime_error("Failed to create reset command pool");
    }

    if (encodeQueue)
    {
        VkCommandPoolCreateInfo encodeCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .queueFamilyIndex = physicalHandle.findQueueFamilyIndex(VK_QUEUE_VIDEO_ENCODE_BIT_KHR).value(),
        };
        if (vkCreateCommandPool(m_handle, &resetCreateInfo, nullptr, &commandPoolEncode))
            throw std::runtime_error("Failed to create reset command pool");
    }
#endif
}
