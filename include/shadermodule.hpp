#pragma once

#include <memory>
#include <vector>

#include "resource.hpp"

#include "graphicsapi.hpp"

class IShaderModule : public ILocalResource
{
public:
	virtual ~IShaderModule() {}

	[[nodiscard]] static std::shared_ptr<IShaderModule> instantiate(const EGraphicsAPI graphicsApi = EGraphicsAPI::VULKAN);
};