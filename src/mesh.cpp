#include "renderer.hpp"
#include "utils/multithread/globalthreadpool.hpp"

#include "mesh.hpp"

void Mesh::cpuLoad()
{
	vertices = {
	{ { 0.0f, -0.5f}, Color::red },
	{ { 0.5f,  0.5f}, Color::green },
	{ {-0.5f,  0.5f}, Color::blue }
	};
}

void Mesh::cpuUnload()
{
	vertices.clear();
}

const Vertex* Mesh::data() const
{
	return vertices.data();
}

void MeshRenderer::create(IHostResource* host)
{
	Utils::GlobalThreadPool::addTask([&]() {
		vbo = &rdr.createVertexBufferObject(3, ((Mesh*)host)->data());
		}, false);
}