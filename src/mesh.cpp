#include "mesh.hpp"

Mesh::Mesh(Renderer* rdr)
{
	local = new GPUMesh(rdr);
}

void Mesh::cpuLoad()
{
	vertices = {
	{ { 0.0f, -0.5f}, Color::red },
	{ { 0.5f,  0.5f}, Color::green },
	{ {-0.5f,  0.5f}, Color::blue }
	};

	//rdr.createVertexBufferObject(3, vertices);
}