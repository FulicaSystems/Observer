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

void Mesh::gpuLoad()
{
	Utils::GlobalThreadPool::addTask([=, this]() {
		local = highRenderer.api->create<IVertexBuffer>(getVertexNum(), getRawData());
		highRenderer.addVBO(std::dynamic_pointer_cast<IVertexBuffer>(local));
		loaded.test_and_set();
		}, false);
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