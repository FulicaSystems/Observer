#pragma once

#include <vector>
#include <deque>
#include <fstream>

#include <glad/vulkan.h>
#include <stdexcept>

#include "graphicsobject.hpp"

#include "commandbuffer.hpp"
#include "vertexbuffer.hpp"

static std::vector<char> readBinaryFile(const std::string& filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);
	if (!file.is_open())
		throw std::runtime_error("Failed to open file");

	size_t fileSize = static_cast<size_t>(file.tellg());
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();
	return buffer;
}

/**
 * Rendering pipeline using a specific logical device.
 */
class GraphicsPipeline : public IDerived<GraphicsPipeline, IGraphicsObject>
{
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

	void recordImageCommandBuffer(CommandBuffer& cb,
		uint32_t imageIndex,
		const std::deque<std::shared_ptr<VertexBuffer>>& vbos);

public:
	void create(LowRenderer* api, LogicalDevice* device) override;
	void destroy() override;

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availableModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	VkShaderModule createShaderModule(const std::vector<char>& code);

	void drawFrame(CommandBuffer& cb, const std::deque<std::shared_ptr<VertexBuffer>>& vbos);
};