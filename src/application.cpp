#include <exception>
#include <vector>
#include <iostream>

#include "format.hpp"
#include "lowrenderer.hpp"

#include "utils/checkleak.hpp"

#include "application.hpp"

Application::Application()
{
	glfwInit();
	windowInit();

	CHKLK_ENTRY_SNAP
	// init GLFW for Vulkan
	if (!glfwVulkanSupported())
		throw std::exception("GLFW failed to find the Vulkan loader");

	// gather Vulkan extensions required by GLFW
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;

	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> requiredExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	// give the extensions to the low renderer
	rdr.low.loadExtensions(requiredExtensions);

	// create the Vulkan instance first
	rdr.low.create();

	// create a surface using the instance
	if (glfwCreateWindowSurface(rdr.low.instance, window, nullptr, &rdr.low.surface) != VK_SUCCESS)
		throw std::exception("Failed to create window surface");

	rdr.create();
}

Application::~Application()
{
	rdr.destroy();
	CHKLK_EXIT_SNAP

	glfwDestroyWindow(window);
	glfwTerminate();
}

void Application::windowInit()
{
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(Format::height, Format::width, "Vulkan window", nullptr, nullptr);
	glfwGetFramebufferSize(window, &Format::framebufferHeight, &Format::framebufferWidth);
}

#include "vertex.hpp"
#include "vertexbuffer.hpp"

void Application::loop()
{
	// TODO : store vbos in a scene

	// this buffer is a CPU accessible buffer (temporary buffer to later load to the GPU)
	VertexBuffer stagingVBO = rdr.createFloatingBufferObject(3,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,	// used for memory transfer operation
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	Vertex vertices[] = {
		{ { 0.0f, -0.5f}, Color::red },
		{ { 0.5f,  0.5f}, Color::green },
		{ {-0.5f,  0.5f}, Color::blue }
	};

	rdr.populateBufferObject(stagingVBO, vertices);

	// creating a device local buffer (on a specific GPU)
	VertexBuffer& vbo = rdr.createBufferObject(3,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,	// memory transfer operation
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	// copying the staging buffer data into the device local buffer
	// using a command buffer to transfer the data
	CommandBuffer cbo = rdr.commandPool.createFloatingCommandBuffer();

	// copy the staging buffer (CPU accessible) into the GPU buffer (GPU memory)
	cbo.beginRecord(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
	VkBufferCopy copyRegion = {
		.srcOffset = 0,
		.dstOffset = 0,
		.size = stagingVBO.bufferSize
	};
	vkCmdCopyBuffer(cbo.getVkBuffer(), stagingVBO.buffer, vbo.buffer, 1, &copyRegion);
	cbo.endRecord();

	VkSubmitInfo submitInfo = {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.commandBufferCount = 1,
		.pCommandBuffers = &cbo.getVkBuffer()
	};
	rdr.ldevice.submitCommandToGraphicsQueue(submitInfo);
	rdr.ldevice.waitGraphicsQueue();

	rdr.commandPool.destroyFloatingCommandBuffer(cbo);
	rdr.destroyFloatingBufferObject(stagingVBO);

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		//draw frame
		rdr.render();
	}
}