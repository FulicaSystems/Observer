#pragma once

//#include <glad/gl.h>
// TODO : use GLuint instead of unsigned int

#include "vertexbufferlocaldesc.hpp"

class VertexBufferDesc_Gl : public IVertexBufferLocalDesc
{
public:
	// vertex buffer object
	unsigned int vbo;
	unsigned int vao;
};