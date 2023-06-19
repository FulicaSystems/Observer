#pragma once

#include "vertexbuffer.hpp"

typedef unsigned int GLuint;

class VertexBufferDesc_Gl : public IVertexBufferLocalDesc
{
public:
	// vertex buffer object
	GLuint vbo;
	GLuint vao;

	~VertexBufferDesc_Gl() override {};
};