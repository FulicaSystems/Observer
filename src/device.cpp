#include "buffer.hpp"
#include "shadermodule.hpp"

#include "device.hpp"



template<>
std::shared_ptr<Buffer> LogicalDevice::create<Buffer>() const
{
	std::shared_ptr<Buffer> out = std::make_shared<Buffer>();

	VkBufferCreateInfo createInfo = {
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		// TODO : fill this struct with args
	};


	vkCreateBuffer(handle, &createInfo, nullptr, &out->handle);

	return out;
}
template<>
void LogicalDevice::destroy<Buffer>(std::shared_ptr<Buffer>& data) const
{
	vkDestroyBuffer(handle, data->handle, nullptr);
}



template<>
std::shared_ptr<ShaderModule> LogicalDevice::create<ShaderModule>() const
{
	// TODO : create shader module
	return std::make_shared<ShaderModule>();
}
template<>
void LogicalDevice::destroy<ShaderModule>(std::shared_ptr<ShaderModule>& pData) const
{
	vkDestroyShaderModule(handle, pData->handle, nullptr);
}