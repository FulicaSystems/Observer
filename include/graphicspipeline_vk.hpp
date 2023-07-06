#pragma once

#include <vector>
#include <deque>
#include <stdexcept>

#include <glad/vulkan.h>

#include "utils/derived.hpp"
#include "graphicspipeline.hpp"
#include "commandbuffer_vk.hpp"
#include "vertexbuffer.hpp"

/**
 * Rendering pipeline using a specific logical device.
 */
class GraphicsPipeline_Vk : public IGraphicsPipeline
{
	SUPER(IGraphicsPipeline)

private:
	// swapchain
	VkSwapchainKHR swapchain;
	std::vector<VkImage> swapchainImages;
	VkFormat swapchainImageFormat;
	VkExtent2D swapchainExtent;
	std::vector<VkImageView> swapchainImageViews;

	// pipeline
	VkRenderPass renderPass;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;

	// framebuffer
	// TODO : create framebuffer interface FrameBuffer_Vk : public IFrameBuffer
	std::vector<VkFramebuffer> swapchainFramebuffers;

	// multithreading
	VkSemaphore renderReadySemaphore;
	VkSemaphore renderDoneSemaphore;
	VkFence renderOnceFence;

	void vulkanSwapchain();
	void vulkanImageViews();

	void vulkanGraphicsPipeline();
	void vulkanRenderPass();

	void vulkanFramebuffers();

	void vulkanMultithreadObjects();

	void recordImageCommandBuffer(CommandBuffer_Vk& cb,
		uint32_t imageIndex,
		const std::deque<std::shared_ptr<IVertexBuffer>>& vbos);

public:
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availableModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	void drawFrame(CommandBuffer_Vk& cb, const std::deque<std::shared_ptr<IVertexBuffer>>& vbos);
};