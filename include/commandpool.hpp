#pragma once

#include <glad/vulkan.h>

#include "graphicsobject.hpp"

class CommandPool : public IGraphicsObject
{
private:
	VkCommandPool commandPool;

	void vulkanCommandPool();

public:
	void create() override;
	void destroy() override;
};