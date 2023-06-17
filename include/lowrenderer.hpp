#pragma once

#include <stdexcept>
#include <array>
#include <span>
#include <memory>

class ILowRenderer
{
protected:
	virtual void initGraphicsAPI_Impl(std::span<void*> args) = 0;


	// vertex buffer object manager

	virtual std::shared_ptr<class VertexBuffer> createVertexBuffer_Impl(uint32_t vertexNum,
		const class Vertex* vertices) = 0;

	template<typename... TArgs>
	[[nodiscard]] std::shared_ptr<class VertexBuffer> createBufferObject(uint32_t vertexNum,
		bool mappable = false,
		TArgs&&... args);

	[[nodiscard]] virtual std::shared_ptr<class VertexBuffer> createBufferObject_Impl(uint32_t vertexNum,
		bool mappable,
		std::span<uint32_t> additionalArgs = std::span<uint32_t>()) = 0;
	virtual void populateBufferObject(class VertexBuffer& vbo, const class Vertex* vertices) = 0;
public:
	virtual void destroyBufferObject(class VertexBuffer& vbo) = 0;

public:
	class Renderer* highRenderer = nullptr;

	template<typename... TArgs>
	void initGraphicsAPI(TArgs&&... args)
	{
		std::array<void*, sizeof...(TArgs)> forwardedArgs = { std::forward<TArgs>(args)... };
		initGraphicsAPI_Impl(forwardedArgs);
	}


	// buffer manager

	// TODO : find a way to use template specialization (create<VertexBuffer>(), create<CommandBuffer>())

	//template<typename TType, typename... TArgs>
	//std::shared_ptr<TType> create(TArgs&&... args)
	//{
	//	static_assert(false, "Either invalid or undefined type creation");
	//}

	template<typename... TArgs>
	std::shared_ptr<class VertexBuffer> create(TArgs&&... args)
	{
		return createVertexBuffer_Impl(std::forward<TArgs>(args)...);
	}

	//template<typename... TArgs>
	//std::shared_ptr<class CommandBuffer> create<class CommandBuffer, TArgs...>(TArgs&&... args)
	//{
	//	// Impl
	//}
};

template<typename... TArgs>
inline std::shared_ptr<class VertexBuffer> ILowRenderer::createBufferObject(uint32_t vertexNum, bool mappable, TArgs&&... args)
{
	std::array<uint32_t, sizeof...(TArgs)> forwardedArgs = { std::forward<TArgs>(args)... };
	return createBufferObject_Impl(vertexNum, mappable, forwardedArgs);
}