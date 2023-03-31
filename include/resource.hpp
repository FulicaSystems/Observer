#pragma once

class IGPUResource
{
private:
	class Renderer* rdr = nullptr;

public:
	// do not forget to declare the constructor in the derived class
	IGPUResource(class Renderer* rdr) : rdr(rdr) {}

	virtual void create() = 0;
	virtual void destroy() = 0;
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

		local->create();
	}

	virtual void cpuUnload() = 0;
	virtual void gpuUnload()
	{
		if (!local)
			return;

		local->destroy();
	}
};