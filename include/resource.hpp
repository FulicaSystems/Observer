#pragma once

#include <filesystem>

// resources used for rendering with GPU
// GPU device local
class ILocalResource
{
protected:
	class Renderer& rdr;

public:
	ILocalResource(class Renderer& rdr) : rdr(rdr) {}

	virtual void create(class IHostResource* host) = 0;
	virtual void destroy(class IHostResource* host) = 0;
};


// host accessible resource
// CPU host
class IHostResource
{
private:
	const char* name = "";
	std::filesystem::path filepath = "";

protected:
	// representation of this resource on the graphics device (GPU)
	// it may not be host accessible (CPU)
	ILocalResource* local = nullptr;

public:
	IHostResource(const char*&& name,
		const char*&& filepath,
		ILocalResource* local)
		: name(name), filepath(filepath), local(local) {}

	virtual ~IHostResource()
	{
		gpuUnload();
		if (local) delete local;
	}

	virtual void cpuLoad() = 0;
	virtual void gpuLoad() { if (local) local->create(this); }

	virtual void cpuUnload() = 0;
	virtual void gpuUnload() { if (local) local->destroy(this); }
};