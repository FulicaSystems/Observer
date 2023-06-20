#pragma once

#include <vector>
#include <memory>

#include "utils/derived.hpp"

#include "resource.hpp"

class Shader : public IHostResource
{
	SUPER(IHostResource)

private:

public:
	std::vector<char> vs;
	std::vector<char> fs;

	void cpuLoad() override;
	void gpuLoad() override;

	void cpuUnload() override;
};