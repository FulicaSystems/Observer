#include "graphics/device/device.hpp"

#include "mesh.hpp"

void Mesh::load()
{
    vertices.reserve(3);
    vertices.push_back(Vertex({{0.0f, -0.5f, 0.f}, Color::red}));
    vertices.push_back(Vertex({{0.5f, 0.5f, 0.f}, Color::green}));
    vertices.push_back(Vertex({{-0.5f, 0.5f, 0.f}, Color::blue}));

    loaded.test_and_set();
}

void Mesh::unload()
{
    vertices.clear();
}

void GPUMesh::load()
{
    auto base = (Mesh *)host;
    vertexCount = base->getVertexCount();
    vertices = base->getRawData();
    bufferSize = base->getDataSize();
    buffer = device.create<Buffer>();

    loaded.test_and_set();
}

void GPUMesh::unload()
{
    device.destroy<Buffer>(buffer);
}