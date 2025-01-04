#include "context.hpp"
#include "physical_device.hpp"

#include "device.hpp"

LogicalDevice::LogicalDevice(const Context& cx, const PhysicalDevice& physicalDevice)
: cx(cx),  physicalHandle(physicalDevice)
{
}

LogicalDevice::~LogicalDevice()
{
    // vkDestroyCommandPool(m_handle, commandPoolDecode, nullptr);
    vkDestroyCommandPool(m_handle, commandPoolTransient, nullptr);
    vkDestroyCommandPool(m_handle, commandPool, nullptr);
    vkDestroyDevice(m_handle, nullptr);
}

void LogicalDevice::loadDeviceFunctions()
{
    VK_GET_DEVICE_PROC_ADDR(m_handle, vkGetDeviceQueue);
    VK_GET_DEVICE_PROC_ADDR(m_handle, vkDestroyDevice);

    VK_GET_DEVICE_PROC_ADDR(m_handle, vkCreateCommandPool);
    VK_GET_DEVICE_PROC_ADDR(m_handle, vkDestroyCommandPool);
}