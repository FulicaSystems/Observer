#pragma once

#include <vector>
#include <memory>

#include "utils/derived.hpp"

#include "vertex.hpp"
#include "resource.hpp"



class Mesh : public HostResourceABC
{
	SUPER(HostResourceABC)

private:
	std::vector<Vertex> vertices;

public:
	void load() override;
	void unload() override;

	inline const uint32_t getVertexCount() const { return vertices.size(); }
	inline const size_t getDataSize() const { return vertices.size() * sizeof(Vertex); }
	inline constexpr const std::vector<Vertex> getData() const { return vertices; }
	inline constexpr const Vertex* getRawData() const { return vertices.data(); }
};



// TODO : remove include
#include "buffer.hpp"

class GPUMesh : public LocalResourceABC
{
public:
	// CPU accessible data
	uint32_t vertexCount = 0;
	const void* vertices = nullptr;

	// buffer size
	size_t bufferSize = 0;


	// GPU data
	std::shared_ptr<Buffer> buffer;


	void load() override;
	void unload() override;
};