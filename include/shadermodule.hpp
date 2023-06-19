#pragma once

#include <memory>
#include <vector>

#include "graphicsapi.hpp"

class IShaderModuleLocalDesc
{
public:
	virtual ~IShaderModuleLocalDesc() {}
};

class ShaderModule
{
public:
	IShaderModuleLocalDesc* localDesc;

	explicit ShaderModule(IShaderModuleLocalDesc* localDesc) : localDesc(localDesc) {}
	~ShaderModule() { delete localDesc; }

	[[nodiscard]] static std::shared_ptr<ShaderModule> createNew(const EGraphicsAPI graphicsApi = EGraphicsAPI::VULKAN);
};