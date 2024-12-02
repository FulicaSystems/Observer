//#include "buffer.hpp"
//template<>
//std::shared_ptr<Buffer> LogicalDevice::create<Buffer>(const void* createInfo) const
//{
//	auto bufferCreateInfo = (VkBufferCreateInfo*)createInfo;
//	assert(bufferCreateInfo &&
//		bufferCreateInfo->sType == VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO);
//	std::shared_ptr<Buffer> out = std::make_shared<Buffer>();
//	vkCreateBuffer(handle, (VkBufferCreateInfo*)createInfo, nullptr, &out->handle);
//	// TODO : memory allocation
//	return out;
//}
//template<>
//void LogicalDevice::destroy<Buffer>(std::shared_ptr<Buffer>& pData) const
//{
//	vkDestroyBuffer(handle, pData->handle, nullptr);
//}
//
//
//#include "shadermodule.hpp"
//template<>
//std::shared_ptr<ShaderModule> LogicalDevice::create<ShaderModule>(const void* createInfo) const
//{
//	auto shaderModuleCreateInfo = (VkShaderModuleCreateInfo*)createInfo;
//	assert(shaderModuleCreateInfo &&
//		shaderModuleCreateInfo->sType == VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO);
//	std::shared_ptr<ShaderModule> out = std::make_shared<ShaderModule>();
//	vkCreateShaderModule(handle, (VkShaderModuleCreateInfo*)createInfo, nullptr, &out->handle);
//	return out;
//}
//template<>
//void LogicalDevice::destroy<ShaderModule>(std::shared_ptr<ShaderModule>& pData) const
//{
//	vkDestroyShaderModule(handle, pData->handle, nullptr);
//}