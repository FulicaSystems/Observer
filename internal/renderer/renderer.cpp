#include <iostream>

#include "graphics/context.hpp"

#include "data/saved/scene.hpp"

#include "renderer.hpp"

LegacyRendererBackend::LegacyRendererBackend(
    const std::shared_ptr<RendererBackendCreateInfoT> createInfo)
    : RendererBackendABC(createInfo)
{
    auto ci = std::dynamic_pointer_cast<LegacyRendererBackendCreateInfoT>(createInfo);
    assert(ci);

    m_renderPass = ci->renderPass;
}

void RendererBackendABC::swap()
{
    m_currentBackBufferIndex = (m_currentBackBufferIndex + 1) % (uint32_t)m_bufferingType;
}

uint32_t LegacyRendererBackend::acquire(const SwapChain* swapchain) const
{
    auto& bb = m_backBuffers[m_currentBackBufferIndex];
    auto* cx = m_device->getContext();

    cx->WaitForFences(m_device->getHandle(), 1, &bb->inFlightFence, VK_TRUE, UINT64_MAX);
    cx->ResetFences(m_device->getHandle(), 1, &bb->inFlightFence);

    uint32_t index;
    VkResult res = cx->AcquireNextImageKHR(
        m_device->getHandle(), swapchain->getHandle(), UINT64_MAX,
        bb->acquireSemaphore.has_value() ? bb->acquireSemaphore.value() : VK_NULL_HANDLE,
        VK_NULL_HANDLE, &index);
    if (res != VK_SUCCESS)
    {
        std::cerr << "Failed to acquire next image : " << res << std::endl;
        return -1;
    }

    return index;
}

void LegacyRendererBackend::begin(const Framebuffer* framebuffer) const
{
    auto& cb = m_backBuffers[m_currentBackBufferIndex]->commandBuffer;

    vkResetCommandBuffer(cb, 0);

    VkCommandBufferBeginInfo commandBufferBeginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = 0,
        .pInheritanceInfo = nullptr};
    VkResult res = vkBeginCommandBuffer(cb, &commandBufferBeginInfo);
    if (res != VK_SUCCESS)
    {
        std::cerr << "Failed to begin recording command buffer : " << res << std::endl;
        return;
    }

    VkClearValue clearColor = {
        .color = {0.2f, 0.2f, 0.2f, 1.f},
    };
    VkClearValue clearDepth = {
        .depthStencil = {1.f, 0},
    };
    std::array<VkClearValue, 2> clearValues = {clearColor, clearDepth};
    VkRenderPassBeginInfo renderPassBeginInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        // TODO : how to manage multiple render passes ?
        .renderPass = m_renderPass->handle,
        .framebuffer = framebuffer->handle,
        .renderArea = {.offset = {0, 0}, .extent = {framebuffer->width, framebuffer->height}},
        .clearValueCount = static_cast<uint32_t>(clearValues.size()),
        .pClearValues = clearValues.data(),
    };
    vkCmdBeginRenderPass(cb, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport = {
        .x = 0.f,
        .y = 0.f,
        .width = static_cast<float>(framebuffer->width),
        .height = static_cast<float>(framebuffer->height),
        .minDepth = 0.f,
        .maxDepth = 1.f,
    };
    vkCmdSetViewport(cb, 0, 1, &viewport);
    VkRect2D scissor = {
        .offset = {0,                  0                  },
        .extent = {framebuffer->width, framebuffer->height},
    };
    vkCmdSetScissor(cb, 0, 1, &scissor);
}
void LegacyRendererBackend::draw(const std::shared_ptr<Scene> scene) const
{
    auto& cb = m_backBuffers[m_currentBackBufferIndex]->commandBuffer;

    auto s = std::static_pointer_cast<GPUScene>(scene->localResource);
    for (int i = 0; i < s->m_meshRenderStates.size(); ++i)
    {
        auto& rs = s->m_meshRenderStates[i];

        vkCmdBindPipeline(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, rs->pipeline->getHandle());

        auto view = rs->getGPUMesh();
        VkBuffer vbos[] = {view->vertexBuffer->handle};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(cb, 0, 1, vbos, offsets);
        vkCmdBindIndexBuffer(cb, view->indexBuffer->handle, 0, VK_INDEX_TYPE_UINT16);
        vkCmdDrawIndexed(cb, view->indexCount, 1, 0, 0, 0);
    }
}
void LegacyRendererBackend::end() const
{
    auto& cb = m_backBuffers[m_currentBackBufferIndex]->commandBuffer;

    vkCmdEndRenderPass(cb);

    VkResult res = vkEndCommandBuffer(cb);
    if (res != VK_SUCCESS)
        std::cerr << "Failed to record command buffer : " << res << std::endl;
}
void LegacyRendererBackend::submit() const
{
    auto& bb = m_backBuffers[m_currentBackBufferIndex];
    auto& cb = bb->commandBuffer;

    std::vector<VkSemaphore> waitSemaphores;
    if (bb->acquireSemaphore.has_value())
        waitSemaphores.emplace_back(bb->acquireSemaphore.value());
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    VkSemaphore signalSemaphores[] = {bb->renderSemaphore};
    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size()),
        .pWaitSemaphores = waitSemaphores.data(),
        .pWaitDstStageMask = waitStages,
        .commandBufferCount = 1,
        .pCommandBuffers = &cb,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = signalSemaphores,
    };

    VkResult res = vkQueueSubmit(m_device->graphicsQueue, 1, &submitInfo, bb->inFlightFence);
    if (res != VK_SUCCESS)
        std::cerr << "Failed to submit draw command buffer : " << res << std::endl;
}
void LegacyRendererBackend::present(
    const std::vector<std::pair<const SwapChain*, uint32_t>> swapchainsAndImageIndices) const
{
    auto& bb = m_backBuffers[m_currentBackBufferIndex];

    std::vector<VkSwapchainKHR> swapchains(swapchainsAndImageIndices.size());
    std::vector<uint32_t> imageIndices(swapchainsAndImageIndices.size());
    for (int i = 0; i < swapchainsAndImageIndices.size(); ++i)
    {
        swapchains[i] = swapchainsAndImageIndices[i].first->getHandle();
        imageIndices[i] = swapchainsAndImageIndices[i].second;
    }
    VkSemaphore waitSemaphores[] = {bb->renderSemaphore};
    VkPresentInfoKHR presentInfo = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = waitSemaphores,
        .swapchainCount = static_cast<uint32_t>(swapchains.size()),
        .pSwapchains = swapchains.data(),
        .pImageIndices = imageIndices.data(),
        .pResults = nullptr,
    };

    VkResult res = vkQueuePresentKHR(m_device->presentQueue, &presentInfo);
    if (res != VK_SUCCESS)
        std::cerr << "Failed to present : " << res << std::endl;
}

uint32_t DynamicRendererBackend::acquire(const SwapChain* swapchain) const
{
    return 0;
}

void DynamicRendererBackend::begin(const Framebuffer* framebuffer) const
{
}
void DynamicRendererBackend::draw(const std::shared_ptr<Scene> scene) const
{
}
void DynamicRendererBackend::end() const
{
}
void DynamicRendererBackend::submit() const
{
}
void DynamicRendererBackend::present(
    const std::vector<std::pair<const SwapChain*, uint32_t>> swapchainsAndImageIndices) const
{
}
Renderer::Renderer(RendererCreateInfoT createInfo)
{
    m_backend = std::move(createInfo.backend);
}

void Renderer::render(const Framebuffer* framebuffer, const std::shared_ptr<Scene> scene)
{
    m_backend->begin(framebuffer);
    m_backend->draw(scene);
    m_backend->end();

    m_backend->submit();
}

void Renderer::swap()
{
    m_backend->swap();
}

RendererBackendABC::RendererBackendABC(const std::shared_ptr<RendererBackendCreateInfoT> createInfo)
    : m_device(createInfo->device)
{
    m_backBuffers.reserve((int)createInfo->bufferingType);
    for (int i = 0; i < (int)createInfo->bufferingType; ++i)
    {
        m_backBuffers.emplace_back(m_device->createBackBufferAOS(BackBufferCreateInfoT{
            .type = createInfo->bufferingType,
            .bHasAcquireSemaphore = true,
            .bSignaled = true,
        }));
    }
}
