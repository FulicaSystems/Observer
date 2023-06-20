#pragma once

#include <filesystem>
#include <memory>

#include "utils/property.hpp"

// resources used for rendering with GPU
// GPU device local
class ILocalResource
{
protected:
	ILocalResource() = default;
public:
	virtual ~ILocalResource() {}
};

// host accessible resource
// CPU host
class IHostResource
{
private:
	const char* name = "";

protected:
	std::filesystem::path filepath = "";

	// reference to the high renderer
	class Renderer& highRenderer;


public:
	std::shared_ptr<ILocalResource> local = nullptr;

	// flag that tells if the resource is fully loaded (CPU and GPU)
	std::atomic_flag loaded = ATOMIC_FLAG_INIT;

	IHostResource(const char*&& name,
		const char*&& filepath,
		class Renderer& highRenderer)
		: name(name), filepath(filepath), highRenderer(highRenderer) {}

	~IHostResource()
	{
		gpuUnload();
		cpuUnload();
	}

	virtual void cpuLoad() = 0;
	virtual void gpuLoad() = 0;

	virtual void cpuUnload() {}
	virtual void gpuUnload() { local.reset(); }
};