#pragma once

#include <vector>
#include <memory>

#include "utils/derived.hpp"

#include "vertex.hpp"
#include "resource.hpp"

class MeshRenderer : public ILocalResource
{
	SUPER(ILocalResource)

private:
	std::shared_ptr<class IVertexBuffer> vbo = nullptr;

public:
	void create(class IHostResource* host) override;
	void destroy(class IHostResource* host) override;
};

class Mesh : public IHostResource
{
	SUPER(IHostResource)

private:
	std::vector<Vertex> vertices;

public:
	void cpuLoad() override;
	void cpuUnload() override;

	const uint32_t getVertexNum() const;
	const Vertex* getRawData() const;
};