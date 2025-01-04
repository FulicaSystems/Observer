#pragma once

#include <iostream>
#include <memory>
#include <vector>

#include <vulkan/vulkan.h>

class Context;

class Instance
{
  private:
    const Context &cx;

  private:
    std::unique_ptr<VkInstance> m_handle;

    std::unique_ptr<VkDebugUtilsMessengerEXT> m_debugMessenger;

  private:
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                        VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                        const VkDebugUtilsMessengerCallbackDataEXT *callbackData,
                                                        void *userData)
    {
        std::cerr << "[Validation Layer, ";
        if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
            std::cerr << "VERBOSE, ";
        else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
            std::cerr << "INFO, ";
        else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
            std::cerr << "WARNING, ";
        else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
            std::cerr << "ERROR, ";

        if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT)
            std::cerr << "GENERAL] : ";
        else if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)
            std::cerr << "VALIDATION] : ";
        else if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)
            std::cerr << "PERFORMANCE] : ";

        std::cerr << callbackData->pMessage << std::endl;
        return VK_FALSE;
    }

  public:
    Instance() = delete;
    Instance(const Instance &) = delete;
    Instance &operator=(const Instance &) = delete;
    Instance(Instance &&) = delete;
    Instance &operator=(Instance &&) = delete;

    Instance(const Context &cx);

    ~Instance();

    void createDebugMessenger();
    void destroyDebugMessenger();

  public:
    [[nodiscard]] inline VkInstance getHandle() const
    {
        return *m_handle;
    }
};