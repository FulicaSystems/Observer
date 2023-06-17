#pragma once

#include <vector>
#include <memory>

#include "lowrenderer.hpp"

/**
 * Low level rendering instance.
 */
class LowRenderer_Gl : public ILowRenderer
{
private:
	void initGraphicsAPI_Impl(std::span<void*> args) override;


	// vertex buffer object

	std::shared_ptr<class VertexBuffer> createVertexBuffer_Impl(uint32_t vertexNum,
		const class Vertex* vertices) override;

	[[nodiscard]] std::shared_ptr<class VertexBuffer> createBufferObject_Impl(uint32_t vertexNum,
		bool mappable,
		std::span<uint32_t> additionalArgs = std::span<uint32_t>()) override;
	void populateBufferObject(class VertexBuffer& vbo, const class Vertex* vertices) override;
public:
	void destroyBufferObject(class VertexBuffer& vbo) override;
};