#pragma once

#include <optional>

#include <glad/vulkan.h>

#include "device.hpp"

std::optional<uint32_t> find_queue_family(PhysicalDevice physicalDevice, VkQueueFlags capabilities)
{
	int i = 0;
	for (auto family : physicalDevice.queueFamilies)
	{
		if (family.queueFlags & capabilities)
			return std::optional<uint32_t>(i);

		++i;
	}
}