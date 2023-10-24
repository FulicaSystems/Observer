#include "device.hpp"

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
	auto asset = std::make_shared<GPUMesh>();
	asset->vertexCount = getVertexCount();
	asset->vertices = getRawData();
	asset->bufferSize = vertices.size() * sizeof(Vertex);
	asset->buffer = device.create<Buffer>();

	local = asset;
	loaded.test_and_set();
}

void Mesh::cpuUnload()
{
	vertices.clear();
}

void Mesh::gpuUnload()
{
	device.destroy<Buffer>(((GPUMesh*)local.get())->buffer);
}

const uint32_t Mesh::getVertexCount() const
{
	return vertices.size();
}

constexpr const Vertex* Mesh::getRawData() const
{
	return vertices.data();
}