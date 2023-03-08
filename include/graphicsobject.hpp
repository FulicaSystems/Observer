#pragma once

class LowRenderer;
class LogicalDevice;

// Graphics object that can be created (initialized) and destroyed
class IGraphicsObject
{
protected:
	// graphics API
	LowRenderer* api = nullptr;
	LogicalDevice* device = nullptr;

public:
	/**
	 * Create the graphics object.
	 */
	virtual void create(LowRenderer* api, LogicalDevice* device)
	{
		this->api = api;
		this->device = device;
	}

	/**
	 * Destroy the graphics object.
	 */
	virtual void destroy() = 0;
};

#include "utils/derived.hpp"