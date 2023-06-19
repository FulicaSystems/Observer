#pragma once

#include <filesystem>

// resources used for rendering with GPU
// GPU device local
class ILocalResource
{
protected:
	class Renderer& highRenderer;

public:
	ILocalResource(class Renderer& highRenderer) : highRenderer(highRenderer) {}

	virtual void create(class IHostResource* host) = 0;
	virtual void destroy(class IHostResource* host) = 0;
};


// host accessible resource
// CPU host
class IHostResource
{
private:
	const char* name = "";

protected:
	std::filesystem::path filepath = "";

	// representation of this resource on the graphics device (GPU)
	// it may not be host accessible (CPU)
	ILocalResource* local = nullptr;

public:
	// flag that tells if the resource is fully loaded (CPU and GPU)
	std::atomic_flag loaded = ATOMIC_FLAG_INIT;

	IHostResource(const char*&& name,
		const char*&& filepath,
		ILocalResource* local)
		: name(name), filepath(filepath), local(local) {}

	~IHostResource()
	{
		gpuUnload();
		cpuUnload();
		if (local) delete local;
	}

	virtual void cpuLoad() = 0;
	virtual void gpuLoad() { if (local) local->create(this); }

	virtual void cpuUnload() {}
	virtual void gpuUnload() { if (local) local->destroy(this); }
};