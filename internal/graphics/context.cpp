#pragma once

#include <iostream>

#include "device/device.hpp"
#include "instance.hpp"

#include "context.hpp"

ContextDLL::ContextDLL(const ContextCreateInfoT createInfo) : ContextABC(createInfo)
{
    m_loader = std::make_unique<f6::bin::DynamicLibraryLoader>("vulkan-1");
    InstanceSymbolsLoaderT::load(this, m_loader.get());
    DeviceSymbolsLoaderT::load(this, m_loader.get());
}

ContextABC::ContextABC(const ContextCreateInfoT createInfo)
    : m_applicationName(createInfo.applicationName),
      m_applicationVersion(createInfo.applicationVersion),
      m_engineVersion(createInfo.engineVersion), ci(createInfo)

{
#ifndef NDEBUG
    ci.instanceExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    ci.instanceExtensions.emplace_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
#endif

    enumerateAvailableInstanceLayers();
    enumerateAvailableInstanceExtensions();
}

std::vector<std::string> ContextABC::enumerateAvailableInstanceLayers(const bool bDump) const
{
    uint32_t layerCount = 0;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<std::string> out;
    out.reserve(layerCount);

    std::vector<VkLayerProperties> layers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, layers.data());
    if (bDump)
        std::cout << "available layers : " << layerCount << '\n';
    for (const auto& layer : layers)
    {
        if (bDump)
            std::cout << '\t' << layer.layerName << '\n';

        out.push_back(layer.layerName);
    }
    return out;
}
std::vector<std::string> ContextABC::enumerateAvailableInstanceExtensions(const bool bDump) const
{
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::vector<std::string> out;
    out.reserve(extensionCount);

    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
    if (bDump)
        std::cout << "available instance extensions : " << extensionCount << '\n';
    for (const auto& extension : extensions)
    {
        if (bDump)
            std::cout << '\t' << extension.extensionName << '\n';

        out.push_back(extension.extensionName);
    }
    return out;
}
