#pragma once

#include <memory>
#include <deque>

#include "utils/derived.hpp"

#include "lowrenderer.hpp"

/**
 * Low level rendering instance.
 */
class LowRenderer_Gl : public ILowRenderer
{
	SUPER(ILowRenderer)

private:
	// every created buffer objects
	std::deque<std::shared_ptr<IVertexBuffer>> vbos;

	void initGraphicsAPI_Impl(std::span<void*> args) override;
public:
	void terminateGraphicsAPI() override {}


private:
	// vertex buffer object
	[[nodiscard]] std::shared_ptr<class IVertexBuffer> createVertexBufferObject_Impl(uint32_t vertexNum,
		bool mappable,
		std::span<uint32_t> additionalArgs = std::span<uint32_t>()) override;
	void populateVertexBufferObject(class IVertexBuffer& vbo, const struct Vertex* vertices) override;
	[[nodiscard]] std::shared_ptr<class IVertexBuffer> createVertexBuffer_Impl(uint32_t vertexNum,
		const struct Vertex* vertices) override;
	void destroyVertexBuffer_Impl(std::shared_ptr<class IVertexBuffer> ptr) override;


	// shader module
	[[nodiscard]] std::shared_ptr<class IShaderModule> createShaderModule_Impl(size_t vsSize,
		size_t fsSize,
		char* vs,
		char* fs) override { return nullptr; }
	void destroyShaderModule_Impl(std::shared_ptr<class IShaderModule> ptr) override {}


public:
	void createAPIInstance(const char* appName) override {}
	void destroyAPIInstance() override {}

	void initRendererModules() override {}
	void terminateRendererModules() override {}

	IVertexBuffer& addVBO(std::shared_ptr<IVertexBuffer> vbo) override;
	void renderFrame() override;
};