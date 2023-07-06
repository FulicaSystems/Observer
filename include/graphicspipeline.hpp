#pragma once

#include <atomic>
#include <memory>

class IGraphicsPipeline
{
public:
	std::atomic_flag readyToDraw = ATOMIC_FLAG_INIT;
	std::shared_ptr<class Shader> shader = nullptr;

	virtual ~IGraphicsPipeline() {}
};