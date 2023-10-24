#pragma once

#include <filesystem>
#include <memory>


// host accessible resource
// CPU host
class IHostResource
{
protected:
	const class LogicalDevice& device;


private:
	const char* name = "";

protected:
	std::filesystem::path filepath = "";


public:
	std::shared_ptr<class ILocalResource> local = nullptr;

	// flag that tells if the resource is fully loaded (CPU and GPU)
	std::atomic_flag loaded = ATOMIC_FLAG_INIT;



	IHostResource(const char*&& name,
		const char*&& filepath,
		class LogicalDevice& device)
		: name(name), filepath(filepath), device(device) {}
	// resources should be unloaded when destroyed (cpuUnload() and gpuUnload())
	virtual ~IHostResource() {};


	virtual void cpuLoad() = 0;
	virtual void gpuLoad() = 0;

	virtual void cpuUnload() = 0;
	virtual void gpuUnload() = 0;
};


/**
 * resources used for rendering with GPU
 * GPU device local
 */
class ILocalResource
{
private:

public:
	virtual ~ILocalResource() = 0;
};