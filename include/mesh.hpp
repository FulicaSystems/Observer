#pragma once

#include <vector>

#include "utils/derived.hpp"

#include "vertex.hpp"
#include "resource.hpp"

class MeshRenderer : public IGPUResource
{
	SUPER(IGPUResource)

private:
	struct VertexBuffer* vbo = nullptr;

public:
	void create(class IResource* host) override;
	void destroy(class IResource* host) override {}
};

class Mesh : public IResource
{
	SUPER(IResource)

private:
	std::vector<Vertex> vertices;

public:
	~Mesh() { cpuUnload(); }

	void cpuLoad() override;
	void cpuUnload() override;

	const Vertex* data() const;
};