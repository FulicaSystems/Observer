#include "graphicsdevice_vk.hpp"

#include "shadermoduledesc_vk.hpp"

#include "shadermodule.hpp"

[[nodiscard]] std::shared_ptr<ShaderModule> ShaderModule::createNew(const EGraphicsAPI graphicsApi)
{
	std::shared_ptr<ShaderModule> sh;
	switch (graphicsApi)
	{
	case EGraphicsAPI::OPENGL:
	{
		//sh = std::make_shared<ShaderModule>(new ShaderModuleDesc_Gl());
		break;
	}
	case EGraphicsAPI::VULKAN:
	{
		sh = std::make_shared<ShaderModule>(new ShaderModuleDesc_Vk());

		break;
	}
	default:
		throw std::runtime_error("Invalid graphics API");
	}

	return sh;
}