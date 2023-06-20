#pragma once

#include "vertexbuffer.hpp"

typedef unsigned int GLuint;

class VertexBuffer_Gl : public IVertexBuffer
{
public:
	// vertex buffer object
	GLuint vbo;
	GLuint vao;

	~VertexBuffer_Gl() override {};
};