#include "device.hpp"

LogicalDevice::~LogicalDevice()
{
    vkDestroyCommandPool(handle, commandPoolDecode, nullptr);
    vkDestroyCommandPool(handle, commandPoolTransient, nullptr);
    vkDestroyCommandPool(handle, commandPool, nullptr);
    vkDestroyDevice(handle, nullptr);
}
