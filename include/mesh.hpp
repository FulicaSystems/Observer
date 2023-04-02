#pragma once

#include <vector>

#include "utils/derived.hpp"

#include "vertex.hpp"
#include "resource.hpp"

class GPUMesh : public IDerived<GPUMesh, IGPUResource>
{
private:
	struct VertexBuffer* vbo = nullptr;

public:
	GPUMesh(class Renderer* rdr) : ctor(rdr) {}

	void create(class IResource* host) override;
	void destroy(class IResource* host) override {}
};

class Mesh : public IResource
{
private:
	std::vector<Vertex> vertices;

public:
	Mesh(class Renderer* rdr);

	void cpuLoad() override;
	void cpuUnload() override {}

	const Vertex* data() const;
};