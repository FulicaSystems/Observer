#include "utils/multithread/globalthreadpool.hpp"
#include "renderer.hpp"
#include "lowrenderer.hpp"

#include "mesh.hpp"

void Mesh::cpuLoad()
{
	vertices.reserve(3);
	vertices.push_back(Vertex({ {  0.0f, -0.5f, 0.f }, Color::red }));
	vertices.push_back(Vertex({ {  0.5f,  0.5f, 0.f }, Color::green }));
	vertices.push_back(Vertex({ { -0.5f,  0.5f, 0.f }, Color::blue }));
}

void Mesh::cpuUnload()
{
	vertices.clear();
}

const uint32_t Mesh::getVertexNum() const
{
	return vertices.size();
}

const Vertex* Mesh::getRawData() const
{
	return vertices.data();
}

void MeshRenderer::create(IHostResource* host)
{
	Mesh* hostResource = (Mesh*)host;

	Utils::GlobalThreadPool::addTask([=, this]() {
		vbo = highRenderer.api->create<IVertexBuffer>(hostResource->getVertexNum(), hostResource->getRawData());
		highRenderer.addVBO(vbo);
		hostResource->loaded.test_and_set();
		}, false);
}

void MeshRenderer::destroy(IHostResource* host)
{
	vbo.reset();
}