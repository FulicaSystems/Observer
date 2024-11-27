#pragma once

#include <vulkan/vulkan.h>

class LogicalDevice
{
	friend class PhysicalDevice;
	friend class std::unique_ptr<LogicalDevice>;
	friend std::unique_ptr<LogicalDevice> std::make_unique<LogicalDevice>();
	friend class DeviceSelector;

private:
	LogicalDevice() = default;

public:
	VkDevice handle;

	VkQueue graphicsQueue;
	VkQueue presentQueue;
	VkQueue decodeQueue;

	// reset command pool
	VkCommandPool commandPool;
	// transient command pool
	VkCommandPool commandPoolTransient;
	// decode reset command pool
	VkCommandPool commandPoolDecode;



	~LogicalDevice()
	{
		//vkDestroyCommandPool(handle, commandPoolDecode, nullptr);
		vkDestroyCommandPool(handle, commandPoolTransient, nullptr);
		vkDestroyCommandPool(handle, commandPool, nullptr);
		vkDestroyDevice(handle, nullptr);
	}



	template<class TDataType>
	std::shared_ptr<TDataType> create(const void* createInfo) const { throw std::runtime_error("Use template specialization"); }
	template<class TDataType>
	void destroy(std::shared_ptr<TDataType>& pData) const { throw std::runtime_error("Use template specialization"); }



	template<>
	std::shared_ptr<class Buffer> create<class Buffer>(const void* createInfo) const;
	template<>
	void destroy<class Buffer>(std::shared_ptr<class Buffer>& pData) const;

	template<>
	std::shared_ptr<class ShaderModule> create<class ShaderModule>(const void* createInfo) const;
	template<>
	void destroy<class ShaderModule>(std::shared_ptr<class ShaderModule>& pData) const;

	// TODO : create<Image>
};



struct SwapchainSupport
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;

	bool tryFindFormat(const VkFormat& targetFormat,
		const VkColorSpaceKHR& targetColorSpace,
		VkSurfaceFormatKHR& found)
	{
		for (const auto& format : formats)
		{
			if (format.format == targetFormat && format.colorSpace == targetColorSpace)
			{
				found = format;
				return true;
			}
		}

		return false;
	}

	bool tryFindPresentMode(const VkPresentModeKHR& targetPresentMode,
		VkPresentModeKHR& found)
	{
		for (const auto& presentMode : presentModes)
		{
			if (presentMode == targetPresentMode)
			{
				found = presentMode;
				return true;
			}
		}

		return false;
	}

	VkExtent2D getExtent()
	{
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			return capabilities.currentExtent;
		}
		else
		{
			// arbitrary values (HD ready)
			return VkExtent2D{
				.width = Math::clamp(1366U,
					capabilities.minImageExtent.width,
					capabilities.maxImageExtent.width),
				.height = Math::clamp(768U,
					capabilities.minImageExtent.height,
					capabilities.maxImageExtent.height),
			};
		}
	}
};