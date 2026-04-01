#include <iostream>

#include "graphics/context.hpp"

#include "renderer.hpp"

MultiPassRendererBackend::MultiPassRendererBackend(
    const std::shared_ptr<RendererBackendCreateInfoT> createInfo)
    : RendererBackendABC(createInfo)
{
    auto ci = std::dynamic_pointer_cast<LegacyRendererBackendCreateInfoT>(createInfo);
    assert(ci);

    m_renderPasses = std::move(ci->renderPasses);

    m_framebuffers.resize(m_renderPasses.size());
    int count = ci->colorAttachmentImageViews.size();
    for (int i = 0; i < m_renderPasses.size(); ++i)
    {
        m_framebuffers[i].reserve(count);
        for (int j = 0; j < count; ++j)
        {
            auto fboCreateInfo = FramebufferCreateInfoT{
                .renderPass = m_renderPasses[i].get(),
                .attachments =
                    {
                                  ci->colorAttachmentImageViews[j],
                                  },
                .width = ci->renderingWidth,
                .height = ci->renderingHeight,
            };
            if (ci->depthAttachmentImageView.has_value())
                fboCreateInfo.attachments.emplace_back(ci->depthAttachmentImageView.value());
            m_framebuffers[i].emplace_back(m_device->createFramebuffer(fboCreateInfo));
        }
    }
}

void RendererBackendABC::swap()
{
    m_currentBackBufferIndex = (m_currentBackBufferIndex + 1) % (uint32_t)m_bufferingType;
}

uint32_t MultiPassRendererBackend::acquire()
{
    auto& bb = m_backBuffers[m_currentBackBufferIndex];
    auto* cx = m_device->getContext();

    cx->WaitForFences(m_device->getHandle(), 1, &bb->inFlightFence, VK_TRUE, UINT64_MAX);
    cx->ResetFences(m_device->getHandle(), 1, &bb->inFlightFence);

    VkResult res = cx->AcquireNextImageKHR(
        m_device->getHandle(), m_swapchain->getHandle(), UINT64_MAX,
        bb->acquireSemaphore.has_value() ? bb->acquireSemaphore.value() : VK_NULL_HANDLE,
        VK_NULL_HANDLE, &m_currentImageIndex);
    if (res != VK_SUCCESS)
    {
        std::cerr << "Failed to acquire next image : " << res << std::endl;
        return -1;
    }

    return m_currentImageIndex;
}

void MultiPassRendererBackend::begin()
{
    auto& cb = m_backBuffers[m_currentBackBufferIndex]->commandBuffer;
    auto& fbo = m_framebuffers[m_currentRenderPassIndex][m_currentImageIndex];

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
        .renderPass = m_renderPasses[m_currentRenderPassIndex]->handle,
        .framebuffer = fbo->handle,
        .renderArea = {.offset = {0, 0}, .extent = {fbo->width, fbo->height}},
        .clearValueCount = static_cast<uint32_t>(clearValues.size()),
        .pClearValues = clearValues.data(),
    };
    vkCmdBeginRenderPass(cb, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport = {
        .x = 0.f,
        .y = 0.f,
        .width = static_cast<float>(fbo->width),
        .height = static_cast<float>(fbo->height),
        .minDepth = 0.f,
        .maxDepth = 1.f,
    };
    vkCmdSetViewport(cb, 0, 1, &viewport);
    VkRect2D scissor = {
        .offset = {0,          0          },
        .extent = {fbo->width, fbo->height},
    };
    vkCmdSetScissor(cb, 0, 1, &scissor);
}
void MultiPassRendererBackend::draw(/*const Scene& scene*/)
{
    auto& cb = m_backBuffers[m_currentBackBufferIndex]->commandBuffer;

    for (int i = 0; i < scene->renderStates.size(); ++i)
    {
        vkCmdBindPipeline(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

        VkBuffer vbos[] = {vertexBuffer};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(cb, 0, 1, vbos, offsets);
        vkCmdBindIndexBuffer(cb, indexBuffer, 0, VK_INDEX_TYPE_UINT16);
        vkCmdDrawIndexed(cb, indexCount, 1, 0, 0, 0);
    }
}
void MultiPassRendererBackend::end()
{
    auto& cb = m_backBuffers[m_currentBackBufferIndex]->commandBuffer;

    vkCmdEndRenderPass(cb);

    VkResult res = vkEndCommandBuffer(cb);
    if (res != VK_SUCCESS)
        std::cerr << "Failed to record command buffer : " << res << std::endl;
}
void MultiPassRendererBackend::submit()
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
void MultiPassRendererBackend::present()
{
    auto& bb = m_backBuffers[m_currentBackBufferIndex];

    VkSwapchainKHR swapchains[] = {m_swapchain->getHandle()};
    VkSemaphore waitSemaphores[] = {bb->renderSemaphore};
    VkPresentInfoKHR presentInfo = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = waitSemaphores,
        .swapchainCount = 1,
        .pSwapchains = swapchains,
        .pImageIndices = &m_currentImageIndex,
        .pResults = nullptr,
    };

    VkResult res = vkQueuePresentKHR(m_device->presentQueue, &presentInfo);
    if (res != VK_SUCCESS)
        std::cerr << "Failed to present : " << res << std::endl;
}

uint32_t SinglePassRendererBackend::acquire()
{
    return 0;
}

void SinglePassRendererBackend::begin()
{
}
void SinglePassRendererBackend::draw(/*const Scene& scene*/)
{
}
void SinglePassRendererBackend::end()
{
}
void SinglePassRendererBackend::submit()
{
}
void SinglePassRendererBackend::present()
{
}
Renderer::Renderer(RendererCreateInfoT createInfo)
{
    m_backend = std::move(createInfo.backend);
}

void Renderer::render()
{
    m_backend->acquire();

    m_backend->begin();
    m_backend->draw(/*const Scene& scene*/);
    m_backend->end();

    m_backend->submit();

    m_backend->present();
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
