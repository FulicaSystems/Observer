#pragma once

#include <memory>
#include <optional>

#include <vulkan/vulkan.hpp>

class Semaphore;
class Buffer;

enum class BufferingTypeE
{
    SINGLE_BUFFERING = 1,
    DOUBLE_BUFFERING = 2,
    TRIPLE_BUFFERING = 3,
    COUNT,

} typedef FrameLagE;

typedef std::vector<std::shared_ptr<Semaphore>> SubmissionSemaphores;

struct BackBufferCreateInfoT
{
    BufferingTypeE type = BufferingTypeE::DOUBLE_BUFFERING;
    uint32_t submitCountPerCommandBuffer = 1U;
    bool bFenceStartsSignaled = true;
};

/**
 * @brief back buffer array of structures
 *
 */
struct BackBufferAOST
{
    VkCommandBuffer commandBuffer;

    /**
     * @brief There are as many semaphores as there are command buffers times the submit count
     * (hence the vector of semaphores). There will be a vector of back buffers therefore a certain
     * number of submit per command buffer (same number of "acquire" semaphore/"before submission"
     * semaphore).
     *
     */
    std::optional<SubmissionSemaphores> beforeSubmissionSemaphores;
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
    std::optional<std::vector<SubmissionSemaphores>> beforeSubmissionSemaphores;
    std::vector<VkFence> inFlightFences;
};
