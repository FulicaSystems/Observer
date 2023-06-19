#pragma once

// Low level graphics object that can be created (initialized) and destroyed
class ILowGraphicsObject
{
protected:
	// graphics API
	class ILowRenderer* api = nullptr;
	class ILogicalDevice* device = nullptr;

public:
	/**
	 * Create the graphics object.
	 */
	virtual void create(class ILowRenderer* api, class ILogicalDevice* device)
	{
		this->api = api;
		this->device = device;
	}

	/**
	 * Destroy the graphics object.
	 */
	virtual void destroy() = 0;
};