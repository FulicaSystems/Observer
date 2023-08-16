#include "utils/multithread/globalthreadpool.hpp"
#include "lowrenderer_vk.hpp"
#include "vertexbuffer.hpp"

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
		local = lowrdr.create<IVertexBuffer>(getVertexNum(), getRawData());
		lowrdr.addVBO(std::dynamic_pointer_cast<IVertexBuffer>(local));
		loaded.test_and_set();
		}, false);
}

void Mesh::cpuUnload()
{
	vertices.clear();
}

void Mesh::gpuUnload()
{
}

const uint32_t Mesh::getVertexNum() const
{
	return vertices.size();
}

const Vertex* Mesh::getRawData() const
{
	return vertices.data();
}