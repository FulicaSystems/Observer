#pragma once

#include <vector>
#include <memory>

#include "utils/derived.hpp"

#include "vertex.hpp"
#include "resource.hpp"

class Mesh : public IHostResource
{
	SUPER(IHostResource)

private:
	std::vector<Vertex> vertices;

public:
	void cpuLoad() override;
	void gpuLoad() override;
	
	void cpuUnload() override;

	const uint32_t getVertexNum() const;
	const Vertex* getRawData() const;
};