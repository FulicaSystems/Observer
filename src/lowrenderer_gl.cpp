#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "vertex.hpp"
#include "vertexbuffer.hpp"

#include "renderer.hpp"

#include "lowrenderer_gl.hpp"

// args[0] should be glfwGetProcAddress
void LowRenderer_Gl::initGraphicsAPI_Impl(std::span<void*> args)
{
	// do not forget to make OpenGL context current using windowing lib

	if (!gladLoadGL((GLADloadfunc)args[0]))
		throw std::exception("Failed to load OpenGL functions");
}


// vertex buffer object

std::shared_ptr<VertexBuffer> LowRenderer_Gl::createVertexBuffer_Impl(uint32_t vertexNum,
	const class Vertex* vertices)
{
	std::shared_ptr<VertexBuffer> vbo = createBufferObject(vertexNum);
	populateBufferObject(*vbo, vertices);

	return vbo;
}

[[nodiscard]] std::shared_ptr<VertexBuffer> LowRenderer_Gl::createBufferObject_Impl(uint32_t vertexNum,
	bool mappable,
	std::span<uint32_t> additionalArgs)
{
	std::shared_ptr<VertexBuffer> outVbo = VertexBuffer::createNew(vertexNum, EGraphicsAPI::OPENGL);
	VertexBufferDesc_Gl* desc = (VertexBufferDesc_Gl*)outVbo->localDesc;

	glGenBuffers(1, &desc->vbo);

	return outVbo;
}

void LowRenderer_Gl::populateBufferObject(VertexBuffer& vbo, const class Vertex* vertices)
{
	VertexBufferDesc_Gl* desc = (VertexBufferDesc_Gl*)vbo.localDesc;

	glBindBuffer(GL_ARRAY_BUFFER, desc->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void LowRenderer_Gl::destroyBufferObject(VertexBuffer& vbo)
{
	VertexBufferDesc_Gl* desc = (VertexBufferDesc_Gl*)vbo.localDesc;

	glDeleteBuffers(1, &desc->vbo);
}