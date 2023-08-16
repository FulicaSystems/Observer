#pragma once

#include <memory>

#include "utils/derived.hpp"

#include "lowrenderer.hpp"

/**
 * Low level rendering instance.
 */
class LowRenderer_Gl : public ILowRenderer
{
	SUPER(ILowRenderer)

private:
	void initGraphicsAPI_Impl(std::span<void*> args) override;


	// vertex buffer object
	[[nodiscard]] std::shared_ptr<class IVertexBuffer> createVertexBufferObject_Impl(uint32_t vertexNum,
		bool mappable,
		std::span<uint32_t> additionalArgs = std::span<uint32_t>()) override;
	void populateVertexBufferObject(class IVertexBuffer& vbo, const struct Vertex* vertices) override;
	std::shared_ptr<class IVertexBuffer> createVertexBuffer_Impl(uint32_t vertexNum,
		const struct Vertex* vertices) override;
	void destroyVertexBuffer_Impl(std::shared_ptr<class IVertexBuffer> ptr) override;


	// shader module
	std::shared_ptr<class IShaderModule> createShaderModule_Impl(size_t vsSize,
		size_t fsSize,
		char* vs,
		char* fs) override { return nullptr; }
	void destroyShaderModule_Impl(std::shared_ptr<class IShaderModule> ptr) override {}
};