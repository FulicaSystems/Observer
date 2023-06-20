#pragma once

#include <memory>

#include "resource.hpp"
#include "graphicsapi.hpp"

class IVertexBuffer : public ILocalResource
{
public:
	// CPU accessible data
	void* vertices = 0;
	uint32_t vertexNum = 0;

	// buffer size
	size_t bufferSize = 0;

	virtual ~IVertexBuffer() {}

	[[nodiscard]] static std::shared_ptr<IVertexBuffer> instantiate(uint32_t vertexNum, const EGraphicsAPI graphicsApi = EGraphicsAPI::VULKAN);
};