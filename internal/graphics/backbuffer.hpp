#pragma once

#include <optional>

#include <vulkan/vulkan.hpp>

class Semaphore;

enum class BufferingTypeE
{
    SINGLE_BUFFERING = 1,
    DOUBLE_BUFFERING = 2,
    TRIPLE_BUFFERING = 3,
    COUNT,
} typedef FrameLagE;

struct BackBufferCreateInfoT
{
    BufferingTypeE type = BufferingTypeE::DOUBLE_BUFFERING;
    bool bHasBeforeSubmissionSemaphore = true;
    bool bFenceStartsSignaled = true;
};

/**
 * @brief back buffer array of structures
 *
 */
struct BackBufferAOST
{
    VkCommandBuffer commandBuffer;

    std::optional<std::shared_ptr<Semaphore>> beforeSubmissionSemaphore;
    /**
     * @brief this fence ensures that each backbuffer (each command buffer) can only be reused
     * (rerecorded) after being submitted
     *
     */
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
    std::optional<std::vector<std::shared_ptr<Semaphore>>> beforeSubmissionSemaphores;
    std::vector<VkFence> inFlightFences;
};
