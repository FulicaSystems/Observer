#pragma once

class IGPUResource
{
protected:
	class Renderer* rdr = nullptr;

public:
	// do not forget to declare the constructor in the derived class
	IGPUResource(class Renderer* rdr) : rdr(rdr) {}

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
	virtual void cpuLoad() = 0;
	virtual void gpuLoad()
	{
		if (!local)
			return;

		local->create(this);
	}

	virtual void cpuUnload() = 0;
	virtual void gpuUnload()
	{
		if (!local)
			return;

		local->destroy(this);
	}
};