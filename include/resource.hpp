#pragma once

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
protected:
	// representation of this resource on the graphics device (GPU)
	// it may not be host accessible (CPU)
	IGPUResource* local = nullptr;

public:
	IResource(IGPUResource* local) : local(local) {}
	~IResource() { if (local) delete local; }

	virtual void cpuLoad() = 0;
	virtual void gpuLoad() { if (local) local->create(this); }

	virtual void cpuUnload() = 0;
	virtual void gpuUnload() { if (local) local->destroy(this); }
};