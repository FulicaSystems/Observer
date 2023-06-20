#include "graphicsdevice_vk.hpp"

#include "shadermodule_vk.hpp"

#include "shadermodule.hpp"

[[nodiscard]] std::shared_ptr<IShaderModule> IShaderModule::instantiate(const EGraphicsAPI graphicsApi)
{
	std::shared_ptr<IShaderModule> sh;
	switch (graphicsApi)
	{
	case EGraphicsAPI::OPENGL:
	{
		//sh = std::make_shared<ShaderModule_Vk>();
		break;
	}
	case EGraphicsAPI::VULKAN:
	{
		sh = std::make_shared<ShaderModule_Vk>();

		break;
	}
	default:
		throw std::runtime_error("Invalid graphics API");
	}

	return sh;
}