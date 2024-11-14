#pragma once

#include <vector>

#include <glad/vulkan.h>

#include "pipeline.hpp"
#include "window.hpp"



enum class MultipleBufferingE
{
	SINGLE_BUFFERING = 1,
	DOUBLE_BUFFERING = 2,
	TRIPLE_BUFFERING = 3,
	COUNT,
};
class RendererABC
{
protected:
	const LogicalDevice& device;



protected:
	MultipleBufferingE bufferingType = MultipleBufferingE::DOUBLE_BUFFERING;


protected:
	VkDescriptorPool descriptorPool;
	std::unique_ptr<Pipeline> pipeline;


	std::vector<VkCommandBuffer> commandBuffers;

	// TODO : abstract semaphores for easier creation and destruction (and support multiple APIs)
	std::vector<VkSemaphore> drawSemaphores;
	std::vector<VkSemaphore> presentSemaphores;
	// TODO : abstract fences for same reasons
	std::vector<VkFence> inFlightFences;


	RendererABC(const LogicalDevice& device,
		const Swapchain& swapchain)
		: device(device)
	{
		commandBuffers.resize((uint32_t)bufferingType);
		drawSemaphores.resize((uint32_t)bufferingType);
		presentSemaphores.resize((uint32_t)bufferingType);
		inFlightFences.resize((uint32_t)bufferingType);


		VkCommandBufferAllocateInfo allocInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.commandPool = device.commandPool,
			.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			.commandBufferCount = (uint32_t)bufferingType,
		};
		vkAllocateCommandBuffers(device.handle, &allocInfo, commandBuffers.data());
		for (int i = 0; i < (int)bufferingType; ++i)
		{
			VkSemaphoreCreateInfo semaphoreCreateInfo = {
				.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
			};
			vkCreateSemaphore(device.handle, &semaphoreCreateInfo, nullptr, &drawSemaphores[i]);
			vkCreateSemaphore(device.handle, &semaphoreCreateInfo, nullptr, &presentSemaphores[i]);

			VkFenceCreateInfo fenceCreateInfo = {
				.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
				.flags = VK_FENCE_CREATE_SIGNALED_BIT,
			};
			vkCreateFence(device.handle, &fenceCreateInfo, nullptr, &inFlightFences[i]);
		}
	}

public:
	virtual ~RendererABC()
	{
		for (int i = 0; i < (int)bufferingType; ++i)
		{
			vkDestroySemaphore(device.handle, drawSemaphores[i], nullptr);
			vkDestroySemaphore(device.handle, presentSemaphores[i], nullptr);
			vkDestroyFence(device.handle, inFlightFences[i], nullptr);
		}
	}

	//void render(Scene);
	//void present();
};




class MultiPassRenderer : public RendererABC
{
private:
	VkRenderPass renderPass;
	std::vector<VkFramebuffer> framebuffers;

public:
	MultiPassRenderer() = delete;
	MultiPassRenderer(const LogicalDevice& device,
		const Swapchain& swapchain)
		: RendererABC(device, swapchain)
	{
		// render pass
		VkAttachmentDescription colorAttachment = {
			.format = swapchain.imageFormat,
			.samples = VK_SAMPLE_COUNT_1_BIT,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR, // load : what to do with the already existing image on the framebuffer
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE, // store : what to do with the newly rendered image on the framebuffer
			.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
		};

		VkAttachmentReference colorAttachmentRef = {
			.attachment = 0,	// colorAttachment is index 0
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

		if (vkCreateRenderPass(device.handle, &createInfo, nullptr, &renderPass) != VK_SUCCESS)
			throw std::runtime_error("Failed to create render pass");


		// pipeline

		pipeline = std::make_unique<Pipeline>(device, renderPass, "triangle", swapchain.imageExtent);



		// framebuffers
		framebuffers.resize(swapchain.imageViews.size());

		for (size_t i = 0; i < swapchain.imageViews.size(); ++i)
		{
			VkFramebufferCreateInfo createInfo = {
				.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
				.renderPass = renderPass,
				.attachmentCount = 1,
				.pAttachments = &swapchain.imageViews[i],
				.width = swapchain.imageExtent.width,
				.height = swapchain.imageExtent.height,
				.layers = 1
			};

			if (vkCreateFramebuffer(device.handle, &createInfo, nullptr, &framebuffers[i]) != VK_SUCCESS)
				throw std::runtime_error("Failed to create framebuffer");
		}
	}
	~MultiPassRenderer() override
	{
		for (VkFramebuffer& framebuffer : framebuffers)
		{
			vkDestroyFramebuffer(device.handle, framebuffer, nullptr);
		}
		vkDestroyRenderPass(device.handle, renderPass, nullptr);
	}
};
typedef MultiPassRenderer RenderPassBasedRenderer;



class SinglePassRenderer : public RendererABC
{
public:
	SinglePassRenderer() = delete;
	SinglePassRenderer(const LogicalDevice& device,
		const Swapchain& swapchain)
		: RendererABC(device, swapchain)
	{
		throw std::runtime_error("not yet implemented");
	}
	~SinglePassRenderer() override
	{

	}
};
typedef SinglePassRenderer DynamicRenderer;