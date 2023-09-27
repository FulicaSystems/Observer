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
	~Mesh() override
	{
		gpuUnload();
		cpuUnload();
	}

	void cpuLoad() override;
	void gpuLoad() override;
	
	void cpuUnload() override;
	void gpuUnload() override;

	const uint32_t getVertexNum() const;
	const Vertex* getRawData() const;
};


class IVertexBuffer : public ILocalResource
{
public:
	// CPU accessible data
	void* vertices = 0;
	uint32_t vertexNum = 0;

	// buffer size
	size_t bufferSize = 0;


	class IAllocation* alloc;

	// vertex buffer object
	VkBuffer buffer;

	IVertexBuffer()
	{
#ifdef USE_VMA
		alloc = new Alloc_VMA();
#else
		alloc = new Alloc();
#endif
	}
	~IVertexBuffer() override
	{
		delete alloc;
	}
}