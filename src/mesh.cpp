#include "renderer.hpp"
#include "utils/multithread/globalthreadpool.hpp"

#include "mesh.hpp"

void Mesh::cpuLoad()
{
	vertices.reserve(3);
	vertices.push_back(Vertex({{  0.0f, -0.5f, 0.f }, Color::red}));
	vertices.push_back(Vertex({{  0.5f,  0.5f, 0.f }, Color::green}));
	vertices.push_back(Vertex({{ -0.5f,  0.5f, 0.f }, Color::blue}));
}

void Mesh::cpuUnload()
{
	vertices.clear();
}

const Vertex* Mesh::getRawData() const
{
	return vertices.data();
}

void MeshRenderer::create(IHostResource* host)
{
	Utils::GlobalThreadPool::addTask([&]() {
		vbo = &rdr.createVertexBufferObject(3, ((Mesh*)host)->getRawData());
		}, false);
}