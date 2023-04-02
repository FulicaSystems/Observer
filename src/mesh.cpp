#include "renderer.hpp"

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
}

const Vertex* Mesh::data() const
{
	return vertices.data();
}

void GPUMesh::create(IResource* host)
{
	rdr->createVertexBufferObject(3, ((Mesh*)host)->data());
}