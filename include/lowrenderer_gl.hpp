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


	// buffer object

	[[nodiscard]] std::shared_ptr<class IVertexBuffer> createBufferObject_Impl(uint32_t vertexNum,
		bool mappable,
		std::span<uint32_t> additionalArgs = std::span<uint32_t>()) override;
	void populateBufferObject(class IVertexBuffer& vbo, const struct Vertex* vertices) override;
public:
	void destroyBufferObject(class IVertexBuffer& vbo) override;


private:
	// vertex buffer object
	std::shared_ptr<class IVertexBuffer> createVertexBuffer_Impl(uint32_t vertexNum,
		const struct Vertex* vertices) override;

	// shader module
	std::shared_ptr<class IShaderModule> createShaderModule_Impl(size_t vsSize,
		size_t fsSize,
		char* vs,
		char* fs) override { return nullptr; }
	void destroyShaderModule_Impl(std::shared_ptr<class IShaderModule> ptr) override {}
};