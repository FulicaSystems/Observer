#include "context.hpp"

#include "instance.hpp"

Instance::Instance(const InstanceCreateInfoT createInfo) : cx(createInfo.cx)
{
    if (!cx)
        return;

    version appv = cx->getApplicationVersion();
    version engv = cx->getEngineVersion();

    VkApplicationInfo appInfo = {.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                                 .pApplicationName = cx->getApplicationName().c_str(),
                                 .applicationVersion = VK_MAKE_API_VERSION(0, MAJOR(appv), MINOR(appv), PATCH(appv)),
                                 .engineVersion = VK_MAKE_API_VERSION(0, MAJOR(engv), MINOR(engv), PATCH(engv)),
                                 .apiVersion = VK_API_VERSION_1_3};

    auto layers = cx->getLayers();
    auto instanceExtensions = cx->getInstanceExtensions();
    VkInstanceCreateInfo ci = {.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                               .pApplicationInfo = &appInfo,
                               .enabledLayerCount = static_cast<uint32_t>(layers.size()),
                               .ppEnabledLayerNames = layers.data(),
                               .enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size()),
                               .ppEnabledExtensionNames = instanceExtensions.data()};

    VkInstance handle;
    VkResult res = cx->CreateInstance(&ci, nullptr, &handle);
    if (res != VK_SUCCESS)
    {
        std::cerr << "Failed to create Vulkan instance : " << res << std::endl;
        return;
    }

    m_handle = std::make_unique<VkInstance>(handle);

    cx->loadTop(this);

#ifndef NDEBUG
    createDebugMessenger();
#endif

    enumerateAvailablePhysicalDevices();
}

Instance::~Instance()
{
    if (m_debugMessenger)
        cx->DestroyDebugUtilsMessengerEXT(*m_handle, *m_debugMessenger, nullptr);

    cx->DestroyInstance(*m_handle, nullptr);
}

void Instance::createDebugMessenger()
{
    VkDebugUtilsMessengerCreateInfoEXT createInfo = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = debugCallback,
        .pUserData = nullptr};

    VkDebugUtilsMessengerEXT handle;
    VkResult res = cx->CreateDebugUtilsMessengerEXT(*m_handle, &createInfo, nullptr, &handle);
    if (res != VK_SUCCESS)
    {
        std::cerr << "Failed to set up debug messenger : " << res << std::endl;
        return;
    }
    m_debugMessenger = std::make_unique<VkDebugUtilsMessengerEXT>(handle);
}

std::vector<std::string> Instance::enumerateAvailablePhysicalDevices(const bool bDump) const
{
    uint32_t deviceCount = 0;
    cx->EnumeratePhysicalDevices(*m_handle, &deviceCount, nullptr);

    std::vector<std::string> out;
    out.reserve(deviceCount);

    std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
    cx->EnumeratePhysicalDevices(*m_handle, &deviceCount, physicalDevices.data());
    if (bDump)
        std::cout << "available physical devices : " << deviceCount << '\n';
    for (const auto &physicalDevice : physicalDevices)
    {
        VkPhysicalDeviceProperties props;
        cx->GetPhysicalDeviceProperties(physicalDevice, &props);
        if (bDump)
            std::cout << '\t' << props.deviceName << '\n';

        out.push_back(props.deviceName);
    }
    return out;
}

std::optional<VkPhysicalDevice> Instance::getPhysicalDeviceHandleByName(const char *deviceName) const
{
    uint32_t deviceCount = 0;
    cx->EnumeratePhysicalDevices(*m_handle, &deviceCount, nullptr);
    std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
    cx->EnumeratePhysicalDevices(*m_handle, &deviceCount, physicalDevices.data());
    for (const auto &physicalDevice : physicalDevices)
    {
        VkPhysicalDeviceProperties props;
        cx->GetPhysicalDeviceProperties(physicalDevice, &props);

        if (std::strcmp(props.deviceName, deviceName))
            return physicalDevice;
    }
    return std::optional<VkPhysicalDevice>();
}
