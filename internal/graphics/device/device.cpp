#include "context.hpp"
#include "physical_device.hpp"

#include "surface.hpp"
#include "swapchain.hpp"

#include "device.hpp"

LogicalDevice::LogicalDevice(const LogicalDeviceCreateInfoT createInfo)
    : cx(createInfo.context), physicalHandle(createInfo.physicalHandle)
{
    VkResult res = cx->vkCreateDevice(physicalHandle->getHandle(), (VkDeviceCreateInfo *)createInfo.createInfo, nullptr,
                                      &m_handle);
    if (res != VK_SUCCESS)
    {
        std::cerr << "Failed to create logical device : " << res << std::endl;
        return;
    }

    cx->DeviceSymbols2T::load(cx, this);

    retrieveQueues();
    createCommandPools();
}

LogicalDevice::~LogicalDevice()
{
#ifdef ENABLE_VIDEO_TRANSCODE
    if (commandPoolEncode)
        cx->vkDestroyCommandPool(m_handle, commandPoolEncode, nullptr);
    if (commandPoolDecode)
        cx->vkDestroyCommandPool(m_handle, commandPoolDecode, nullptr);
#endif

    cx->vkDestroyCommandPool(m_handle, commandPoolTransient, nullptr);
    cx->vkDestroyCommandPool(m_handle, commandPool, nullptr);

    cx->vkDestroyDevice(m_handle, nullptr);
}

std::unique_ptr<SwapChain> LogicalDevice::createSwapChain(SwapChainCreateInfoT ci) const
{
    VkSurfaceCapabilitiesKHR capabilities = physicalHandle->getSurfaceCapabilities(*ci.surface);
    auto compatibility = physicalHandle->getSurfaceDetails(*ci.surface);
    auto surfaceFormat = compatibility.formats[0];
    auto presentMode = compatibility.presentModes[0];
    int temp;
    auto surfaceFormatOpt = compatibility.findFormat(ci.surfaceFormat.format, ci.surfaceFormat.colorSpace, temp);
    if (surfaceFormatOpt.has_value())
        surfaceFormat = surfaceFormatOpt.value();
    auto presentModeOpt = compatibility.findPresentMode(ci.presentMode, temp);
    if (presentModeOpt.has_value())
        presentMode = presentModeOpt.value();
    auto extent = compatibility.findExtent(ci.extent.width, ci.extent.height);

    uint32_t imageCount = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 && capabilities.maxImageCount < imageCount)
        imageCount = capabilities.maxImageCount;

    VkSwapchainCreateInfoKHR createInfo = {.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
                                           .surface = ci.surface->getHandle(),
                                           .minImageCount = imageCount,
                                           .imageFormat = surfaceFormat.format,
                                           .imageColorSpace = surfaceFormat.colorSpace,
                                           .imageExtent = extent,
                                           .imageArrayLayers = 1,
                                           .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                                           .preTransform = capabilities.currentTransform,
                                           .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
                                           .presentMode = presentMode,
                                           .clipped = VK_TRUE,
                                           .oldSwapchain = VK_NULL_HANDLE};

    std::vector<uint32_t> queueFamilyIndices;
    if (physicalHandle->getGraphicsFamilyIndex().has_value())
        queueFamilyIndices.emplace_back(physicalHandle->getGraphicsFamilyIndex().value());
    if (physicalHandle->getPresentFamilyIndex().has_value())
        queueFamilyIndices.emplace_back(physicalHandle->getPresentFamilyIndex().value());

    if ((physicalHandle->getGraphicsFamilyIndex().has_value() && physicalHandle->getPresentFamilyIndex().has_value()) &&
        physicalHandle->getGraphicsFamilyIndex().value() != physicalHandle->getPresentFamilyIndex().value())
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices.data();
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }

    ci.device = std::make_optional<const LogicalDevice *>(this);
    std::unique_ptr<SwapChain> out = std::make_unique<SwapChain>(ci);
    VkResult res = cx->vkCreateSwapchainKHR(m_handle, &createInfo, nullptr, &out->getHandle());
    if (res != VK_SUCCESS)
    {
        std::cerr << "Failed to create swapchain : " << res << std::endl;
        return nullptr;
    }

    // prepare the swapchain object
    cx->vkGetSwapchainImagesKHR(m_handle, out->getHandle(), &imageCount, nullptr);
    out->images.resize(imageCount);
    cx->vkGetSwapchainImagesKHR(m_handle, out->getHandle(), &imageCount, out->images.data());

    out->imageFormat = surfaceFormat.format;
    out->imageExtent = extent;

    return std::move(out);
}

void LogicalDevice::destroySwapChain(SwapChain &sc) const
{
    for (VkImageView &imageView : sc.imageViews)
    {
        cx->vkDestroyImageView(m_handle, imageView, nullptr);
    }
    cx->vkDestroySwapchainKHR(m_handle, sc.getHandle(), nullptr);
}

void LogicalDevice::retrieveQueues()
{
    auto graphicsFamilyIndex = physicalHandle->getGraphicsFamilyIndex();
    auto presentFamilyIndex = physicalHandle->getPresentFamilyIndex();
#ifdef ENABLE_VIDEO_TRANSCODE
    auto decodeFamilyIndex = physicalHandle->getDecodeFamilyIndex();
    auto encodeFamilyIndex = physicalHandle->getEncodeFamilyIndex();
#endif

    if (graphicsFamilyIndex.has_value())
        cx->vkGetDeviceQueue(m_handle, graphicsFamilyIndex.value(), 0, &graphicsQueue);
    if (presentFamilyIndex.has_value())
        cx->vkGetDeviceQueue(m_handle, presentFamilyIndex.value(), 0, &presentQueue);
#ifdef ENABLE_VIDEO_TRANSCODE
    if (decodeFamilyIndex.has_value())
        cx->vkGetDeviceQueue(m_handle, decodeFamilyIndex.value(), 0, &decodeQueue);
    if (encodeFamilyIndex.has_value())
        cx->vkGetDeviceQueue(m_handle, encodeFamilyIndex.value(), 0, &encodeQueue);
#endif
}

void LogicalDevice::createCommandPools()
{
    VkCommandPoolCreateInfo resetCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = physicalHandle->findQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT).value(),
    };

    if (graphicsQueue)
    {
        VkResult res = cx->vkCreateCommandPool(m_handle, &resetCreateInfo, nullptr, &commandPool);
        if (res != VK_SUCCESS)
        {
            std::cerr << "Failed to create reset command pool : " << res << std::endl;
            return;
        }
        VkCommandPoolCreateInfo transientCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
            .queueFamilyIndex = physicalHandle->findQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT).value(),
        };
        res = cx->vkCreateCommandPool(m_handle, &transientCreateInfo, nullptr, &commandPoolTransient);
        if (res != VK_SUCCESS)
        {
            std::cerr << "Failed to create transient command pool : " << res << std::endl;
            return;
        }
    }

#ifdef ENABLE_VIDEO_TRANSCODE
    if (decodeQueue)
    {
        cx->VkCommandPoolCreateInfo decodeCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .queueFamilyIndex = physicalHandle->findQueueFamilyIndex(VK_QUEUE_VIDEO_DECODE_BIT_KHR).value(),
        };
        res = cx->vkCreateCommandPool(m_handle, &resetCreateInfo, nullptr, &commandPoolDecode);
        if (res != VK_SUCCESS)
        {
            std::cerr << "Failed to create reset command pool : " << res << std::endl;
            return;
        }
    }

    if (encodeQueue)
    {
        cx->VkCommandPoolCreateInfo encodeCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .queueFamilyIndex = physicalHandle->findQueueFamilyIndex(VK_QUEUE_VIDEO_ENCODE_BIT_KHR).value(),
        };
        res = cx->vkCreateCommandPool(m_handle, &resetCreateInfo, nullptr, &commandPoolEncode);
        if (res != VK_SUCCESS)
        {
            std::cerr << "Failed to create reset command pool : " << res << std::endl;
            return;
        }
    }
#endif
}
