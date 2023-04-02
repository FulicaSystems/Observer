#pragma once

#include <filesystem>

// resources used for rendering with GPU
class IGPUResource
{
protected:
	class Renderer& rdr;

public:
	IGPUResource(class Renderer& rdr) : rdr(rdr) {}

	virtual void create(class IResource* host) = 0;
	virtual void destroy(class IResource* host) = 0;
};

class IResource
{
private:
	const char* name = "";
	std::filesystem::path filepath = "";

protected:
	// representation of this resource on the graphics device (GPU)
	// it may not be host accessible (CPU)
	IGPUResource* local = nullptr;

public:
	IResource(const char*&& name,
		const char*&& filepath,
		IGPUResource* local)
		: name(name), filepath(filepath), local(local) {}

	virtual ~IResource()
	{
		gpuUnload();
		if (local) delete local;
	}

	virtual void cpuLoad() = 0;
	virtual void gpuLoad() { if (local) local->create(this); }

	virtual void cpuUnload() = 0;
	virtual void gpuUnload() { if (local) local->destroy(this); }
};