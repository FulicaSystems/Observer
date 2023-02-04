#pragma once

// Graphics object that can be created (initialized) and destroyed
class IGraphicsObject
{
public:
	/**
	 * Create the graphics object.
	 */
	virtual void create() = 0;

	/**
	 * Destroy the graphics object.
	 */
	virtual void destroy() = 0;
};