#pragma once

#include <vector>

#include "utils/derived.hpp"

#include "vertex.hpp"
#include "resource.hpp"

class MeshRenderer : public ILocalResource
{
	SUPER(ILocalResource)

private:
	struct VertexBuffer* vbo = nullptr;

public:
	void create(class IHostResource* host) override;
	void destroy(class IHostResource* host) override {}
};

class Mesh : public IHostResource
{
	SUPER(IHostResource)

private:
	std::vector<Vertex> vertices;

public:
	~Mesh() { cpuUnload(); }

	void cpuLoad() override;
	void cpuUnload() override;

	const int getVertexNum() const;
	const Vertex* getRawData() const;
};