#pragma once

#include <optional>

#include <vulkan/vulkan.hpp>

enum class BufferingTypeE
{
    SINGLE_BUFFERING = 1,
    DOUBLE_BUFFERING = 2,
    TRIPLE_BUFFERING = 3,
    COUNT,
};

struct BackBufferCreateInfoT
{
    BufferingTypeE type = BufferingTypeE::DOUBLE_BUFFERING;
    bool bHasAcquireSemaphore = true;
    bool bSignaled = true;
};

/**
 * @brief back buffer array of structures
 *
 */
struct BackBufferAOST
{
    VkCommandBuffer commandBuffer;

    std::optional<VkSemaphore> acquireSemaphore;
    VkSemaphore renderSemaphore;
    VkFence inFlightFence;

} typedef BackBufferT;

/**
 * @brief back buffers used in an structure of array architecture
 * TODO
 *
 */
struct BackBufferSOAT
{
    BufferingTypeE type = BufferingTypeE::DOUBLE_BUFFERING;

    std::vector<VkCommandBuffer> commandBuffers;

    std::vector<std::optional<VkSemaphore>> acquireSemaphores;
    std::vector<VkSemaphore> renderSemaphores;
    std::vector<VkFence> inFlightFences;
};
