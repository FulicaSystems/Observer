#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "vertex.hpp"
#include "vertexbuffer_gl.hpp"

#include "lowrenderer_gl.hpp"

// args[0] should be glfwGetProcAddress
void LowRenderer_Gl::initGraphicsAPI_Impl(std::span<void*> args)
{
	// do not forget to make OpenGL context current using windowing lib

	if (!gladLoadGL((GLADloadfunc)args[0]))
		throw std::exception("Failed to load OpenGL functions");
}


[[nodiscard]] std::shared_ptr<IVertexBuffer> LowRenderer_Gl::createBufferObject_Impl(uint32_t vertexNum,
	bool mappable,
	std::span<uint32_t> additionalArgs)
{
	std::shared_ptr<VertexBuffer_Gl> outVbo =
		std::dynamic_pointer_cast<VertexBuffer_Gl>(IVertexBuffer::instantiate(vertexNum,
			EGraphicsAPI::OPENGL));

	glGenBuffers(1, &outVbo->vbo);

	return outVbo;
}

void LowRenderer_Gl::populateBufferObject(IVertexBuffer& vbo, const Vertex* vertices)
{
	glBindBuffer(GL_ARRAY_BUFFER, ((VertexBuffer_Gl&)vbo).vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void LowRenderer_Gl::destroyBufferObject(IVertexBuffer& vbo)
{
	glDeleteBuffers(1, &((VertexBuffer_Gl&)vbo).vbo);
}


std::shared_ptr<IVertexBuffer> LowRenderer_Gl::createVertexBuffer_Impl(uint32_t vertexNum,
	const Vertex* vertices)
{
	std::shared_ptr<IVertexBuffer> vbo = createBufferObject(vertexNum);
	populateBufferObject(*vbo, vertices);

	return vbo;
}