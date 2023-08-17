#include <iostream>

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


std::shared_ptr<IVertexBuffer> LowRenderer_Gl::createVertexBufferObject_Impl(uint32_t vertexNum,
	bool mappable,
	std::span<uint32_t> additionalArgs)
{
	std::shared_ptr<VertexBuffer_Gl> outVbo =
		std::dynamic_pointer_cast<VertexBuffer_Gl>(IVertexBuffer::instantiate(vertexNum,
			EGraphicsAPI::OPENGL));

	glGenBuffers(1, &outVbo->vbo);

	return outVbo;
}
void LowRenderer_Gl::populateVertexBufferObject(IVertexBuffer& vbo, const Vertex* vertices)
{
	glBindBuffer(GL_ARRAY_BUFFER, ((VertexBuffer_Gl&)vbo).vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
std::shared_ptr<IVertexBuffer> LowRenderer_Gl::createVertexBuffer_Impl(uint32_t vertexNum,
	const Vertex* vertices)
{
	std::shared_ptr<IVertexBuffer> vbo = createVertexBufferObject(vertexNum);
	populateVertexBufferObject(*vbo, vertices);

	std::shared_ptr<VertexBuffer_Gl> vbogl = std::dynamic_pointer_cast<VertexBuffer_Gl>(vbo);
	glGenVertexArrays(1, &vbogl->vao);
	glBindVertexArray(vbogl->vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbogl->vbo);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FLOAT, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return vbo;
}
void LowRenderer_Gl::destroyVertexBuffer_Impl(std::shared_ptr<IVertexBuffer> ptr)
{
	glDeleteBuffers(1, &((VertexBuffer_Gl&)ptr).vbo);
}

std::shared_ptr<class IShaderModule> LowRenderer_Gl::createShaderModule_Impl(size_t vsSize,
	size_t fsSize,
	char* vs,
	char* fs)
{
	// TODO : SPIR-V support


	GLuint ivs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(ivs, 1, &vs, NULL);
	glCompileShader(ivs);

	GLuint ifs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(ifs, 1, &fs, NULL);
	glCompileShader(ifs);

	int success[2];
	char infoLog[2][512];
	glGetShaderiv(ivs, GL_COMPILE_STATUS, &success[0]);
	glGetShaderiv(ivs, GL_COMPILE_STATUS, &success[1]);

	if (!success[0] || !success[1])
	{
		glGetShaderInfoLog(ivs, 512, NULL, infoLog[0]);
		std::cout << infoLog[0] << std::endl;
		glGetShaderInfoLog(ifs, 512, NULL, infoLog[1]);
		std::cout << infoLog[1] << std::endl;

		throw std::exception("Failed to compile shaders");
	}

	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, ivs);
	glAttachShader(shaderProgram, ifs);
	glLinkProgram(shaderProgram);

	glDeleteShader(ivs);
	glDeleteShader(ifs);
}
void LowRenderer_Gl::destroyShaderModule_Impl(std::shared_ptr<class IShaderModule> ptr)
{
	glDeleteProgram(shaderProgram);
}


IVertexBuffer& LowRenderer_Gl::addVBO(std::shared_ptr<IVertexBuffer> vbo)
{
	// TODO : add mutex for thread safety
	return *vbos.emplace_back(vbo);
}

void LowRenderer_Gl::renderFrame()
{
	glViewport(0, 0, 800, 600);

	glClearColor(0.2f, 0.2f, 0.2f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgram);

	for (int i = 0; i < vbos.size(); ++i)
	{
		const VertexBuffer_Gl& vbo = *(VertexBuffer_Gl*)vbos.at(i).get();
		glBindVertexArray(vbo.vao);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);
	}
}