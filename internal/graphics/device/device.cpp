#include "context.hpp"
#include "physical_device.hpp"

#include "surface.hpp"
#include "swapchain.hpp"

#include "device.hpp"

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

std::unique_ptr<SwapChain> LogicalDevice::createSwapChain(const Surface *presentationSurface) const
{
    SurfaceDetailsT details = physicalHandle.querySurfaceDetails(*presentationSurface);

    VkSurfaceFormatKHR surfaceFormat;
    details.tryFindFormat(VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR, surfaceFormat);
    VkPresentModeKHR presentMode;
    details.tryFindPresentMode(VK_PRESENT_MODE_MAILBOX_KHR, presentMode);
    // TODO : remove arbitraty value (hd ready)
    VkExtent2D extent = details.findExtent(1366U, 768U);

    uint32_t imageCount = details.capabilities.minImageCount + 1;
    if (details.capabilities.maxImageCount > 0 && details.capabilities.maxImageCount < imageCount)
        imageCount = details.capabilities.maxImageCount;

    VkSwapchainCreateInfoKHR createInfo = {.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
                                           .surface = presentationSurface->getHandle(),
                                           .minImageCount = imageCount,
                                           .imageFormat = surfaceFormat.format,
                                           .imageColorSpace = surfaceFormat.colorSpace,
                                           .imageExtent = extent,
                                           .imageArrayLayers = 1,
                                           .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                                           .preTransform = details.capabilities.currentTransform,
                                           .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
                                           .presentMode = presentMode,
                                           .clipped = VK_TRUE,
                                           .oldSwapchain = VK_NULL_HANDLE};

    uint32_t queueFamilyIndices[] = {
        physicalHandle.getGraphicsFamilyIndex().value(),
        physicalHandle.getPresentFamilyIndex().value(),
    };
    if (physicalHandle.getGraphicsFamilyIndex().value() != physicalHandle.getPresentFamilyIndex().value())
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }

    std::unique_ptr<SwapChain> out = std::make_unique<SwapChain>(cx, *this, *presentationSurface);
    if (vkCreateSwapchainKHR(m_handle, &createInfo, nullptr, &out->getHandle()) != VK_SUCCESS)
        throw std::runtime_error("Failed to create swapchain");


    // prepare the swapchain object
    vkGetSwapchainImagesKHR(m_handle, out->getHandle(), &imageCount, nullptr);
    out->images.resize(imageCount);
    vkGetSwapchainImagesKHR(m_handle, out->getHandle(), &imageCount, out->images.data());

    out->imageFormat = surfaceFormat.format;
    out->imageExtent = extent;

    return std::move(out);
}

void LogicalDevice::loadDeviceFunctions()
{
    VK_GET_DEVICE_PROC_ADDR(m_handle, vkGetDeviceQueue);
    VK_GET_DEVICE_PROC_ADDR(m_handle, vkDestroyDevice);

    VK_GET_DEVICE_PROC_ADDR(m_handle, vkCreateCommandPool);
    VK_GET_DEVICE_PROC_ADDR(m_handle, vkDestroyCommandPool);

    VK_GET_DEVICE_PROC_ADDR(m_handle, vkCreateSwapchainKHR);
    VK_GET_DEVICE_PROC_ADDR(m_handle, vkGetSwapchainImagesKHR);
    VK_GET_DEVICE_PROC_ADDR(m_handle, vkDestroyImageView);
    VK_GET_DEVICE_PROC_ADDR(m_handle, vkDestroySwapchainKHR);
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
