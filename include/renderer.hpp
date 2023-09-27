#pragma once

#include <vector>

#include <glad/vulkan.h>

class MultiPassRenderer
{
private:
	VkRenderPass renderPass;

	Pipeline pipeline;

	std::vector<VkFramebuffer> framebuffers;

public:
	MultiPassRenderer() = delete;
	MultiPassRenderer()
	{
		// render pass
		VkAttachmentDescription colorAttachment = {
			.format = pipeline.swapchain.swapchainImageFormat,
			.samples = VK_SAMPLE_COUNT_1_BIT,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,		//load : what to do with the already existing image on the framebuffer
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE,	//store : what to do with the newly rendered image on the framebuffer
			.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
		};

		VkAttachmentReference colorAttachmentRef = {
			.attachment = 0,	//colorAttachment is index 0
			.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
		};

		VkSubpassDescription subpass = {
			.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
			.colorAttachmentCount = 1,
			.pColorAttachments = &colorAttachmentRef
		};

		VkSubpassDependency dependency = {
			.srcSubpass = VK_SUBPASS_EXTERNAL,
			.dstSubpass = 0,
			.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			.srcAccessMask = 0,
			.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
		};

		VkRenderPassCreateInfo createInfo = {
			.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
			.attachmentCount = 1,
			.pAttachments = &colorAttachment,
			.subpassCount = 1,
			.pSubpasses = &subpass,
			.dependencyCount = 1,
			.pDependencies = &dependency
		};

		if (vkCreateRenderPass(logicalDevice.vkdevice, &createInfo, nullptr, &pipeline.renderPass) != VK_SUCCESS)
			throw std::runtime_error("Failed to create render pass");


		// framebuffers
		pipeline.framebuffers.swapchainFramebuffers.resize(pipeline.swapchain.swapchainImageViews.size());

		for (size_t i = 0; i < pipeline.swapchain.swapchainImageViews.size(); ++i)
		{
			VkFramebufferCreateInfo createInfo = {
				.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
				.renderPass = pipeline.renderPass,
				.attachmentCount = 1,
				.pAttachments = &pipeline.swapchain.swapchainImageViews[i],
				.width = pipeline.swapchain.swapchainExtent.width,
				.height = pipeline.swapchain.swapchainExtent.height,
				.layers = 1
			};

			if (vkCreateFramebuffer(logicalDevice.vkdevice, &createInfo, nullptr, &pipeline.framebuffers.swapchainFramebuffers[i]) != VK_SUCCESS)
				throw std::runtime_error("Failed to create framebuffer");
		}
	}
	~MultiPassRenderer()
	{
		for (VkFramebuffer& framebuffer : pipeline.framebuffers.swapchainFramebuffers)
		{
			vkDestroyFramebuffer(logicalDevice.vkdevice, framebuffer, nullptr);
		}
		vkDestroyRenderPass(logicalDevice.vkdevice, pipeline.renderPass, nullptr);
	}
};

class SinglePassRenderer
{
private:
	Pipeline pipeline;

public:
	SinglePassRenderer() = delete;
	SinglePassRenderer()
	{

	}
	~SinglePassRenderer()
	{

	}
};