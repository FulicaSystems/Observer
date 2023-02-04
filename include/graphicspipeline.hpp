#pragma once

#include <vector>
#include <fstream>

#include <glad/vulkan.h>

#include "graphicsobject.hpp"

static std::vector<char> readBinaryFile(const std::string& filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);
	if (!file.is_open())
		throw std::exception("Failed to open file");

	size_t fileSize = static_cast<size_t>(file.tellg());
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();
	return buffer;
}


class GraphicsPipeline : public IGraphicsObject
{
private:
	LogicalDevice& device;

	//swapchain
	VkSwapchainKHR swapchain;
	std::vector<VkImage> swapchainImages;
	VkFormat swapchainImageFormat;
	VkExtent2D swapchainExtent;
	std::vector<VkImageView> swapchainImageViews;

	//pipeline
	VkRenderPass renderPass;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;

	//framebuffer
	std::vector<VkFramebuffer> swapchainFramebuffers;
	VkCommandPool commandPool;
	VkCommandBuffer commandBuffer;

	//multithreading
	VkSemaphore renderReadySemaphore;
	VkSemaphore renderDoneSemaphore;
	VkFence renderOnceFence;

	void vulkanSwapchain();
	void vulkanImageViews();

	void vulkanGraphicsPipeline();
	void vulkanRenderPass();

	void vulkanFramebuffers();
	void vulkanCommandPool();
	void vulkanCommandBuffer();

	void vulkanMultithreadObjects();

	void recordCommandBuffer(VkCommandBuffer cb, uint32_t imageIndex);

public:
	GraphicsPipeline(LogicalDevice& device);

	void create() override;
	void destroy() override;

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availableModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	VkShaderModule createShaderModule(const std::vector<char>& code);

	void drawFrame();
};