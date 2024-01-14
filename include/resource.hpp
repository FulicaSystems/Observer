#pragma once

#include <filesystem>
#include <memory>




struct ResourceLoadInfoI
{
	// device on which the resource will be allocated/created
	const class LogicalDevice& device;

	// resource path if relevant
	std::filesystem::path filepath;
};


class ResourceABC
{
public:
	// flag that tells if the resource is successfully loaded
	std::atomic_flag loaded = ATOMIC_FLAG_INIT;


	virtual ~ResourceABC() = default;


	virtual void load() = 0;
	virtual void unload() = 0;
};



// host accessible resource
// CPU host
class HostResourceABC : public ResourceABC
{
protected:
	const uint64_t index = 0ULL;
	std::filesystem::path filepath = "";
	const ResourceLoadInfoI* loadInfo;


public:
	std::shared_ptr<class LocalResourceABC> local = nullptr;


	HostResourceABC() = delete;
	HostResourceABC(const uint64_t index,
		const ResourceLoadInfoI* loadInfo)
		: index(index), loadInfo(loadInfo) {}


	// unload when destroying
	virtual ~HostResourceABC() override = default;
};


/**
 * resources used for rendering with GPU
 * GPU device local
 */
class LocalResourceABC : public ResourceABC
{
protected:
	const class LogicalDevice& device;
	const HostResourceABC* host;


public:
	LocalResourceABC() = delete;
	LocalResourceABC(const class LogicalDevice& device, const HostResourceABC* host)
		: device(device), host(host) {}



	virtual ~LocalResourceABC() override = default;
};