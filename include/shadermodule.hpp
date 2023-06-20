#pragma once

#include <memory>
#include <vector>

#include "graphicsapi.hpp"

class IShaderModule
{
public:
	virtual ~IShaderModule() {}

	[[nodiscard]] static std::shared_ptr<IShaderModule> instantiate(const EGraphicsAPI graphicsApi = EGraphicsAPI::VULKAN);
};