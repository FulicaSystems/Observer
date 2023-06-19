#pragma once

#include <atomic>
#include <memory>

#include "lowgraphicsobject.hpp"

class IGraphicsPipeline : public ILowGraphicsObject
{
public:
	std::atomic_flag readyToDraw = ATOMIC_FLAG_INIT;
	std::shared_ptr<class Shader> shader = nullptr;
};