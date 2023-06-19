#pragma once

#include <vector>
#include <memory>

#include "utils/derived.hpp"

#include "resource.hpp"

// TODO : copy mesh architecture

class ShaderCompiled : public ILocalResource
{
	SUPER(ILocalResource)

private:

public:
	std::shared_ptr<class ShaderModule> shmodule = nullptr;

	void create(class IHostResource* host) override;
	void destroy(class IHostResource* host) override;
};

class Shader : public IHostResource
{
	SUPER(IHostResource)

private:

public:
	std::vector<char> vs;
	std::vector<char> fs;

	void cpuLoad() override;
	void cpuUnload() override;

	const ShaderModule* getModule() const;
};