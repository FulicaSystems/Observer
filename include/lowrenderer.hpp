#pragma once

#include <stdexcept>
#include <array>
#include <span>
#include <memory>

class ILowRenderer
{
protected:
	virtual void initGraphicsAPI_Impl(std::span<void*> args) = 0;


	// buffer object manager

	template<typename... TArgs>
	[[nodiscard]] std::shared_ptr<class VertexBuffer> createBufferObject(uint32_t vertexNum,
		bool mappable = false,
		TArgs&&... args)
	{
		std::array<uint32_t, sizeof...(TArgs)> forwardedArgs = { std::forward<TArgs>(args)... };
		return createBufferObject_Impl(vertexNum, mappable, forwardedArgs);
	}
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


	// graphics object manager

	template<typename TType, typename... TArgs>
	std::shared_ptr<TType> create(TArgs... args)
	{
		static_assert(sizeof(TType) == 0, "Invalid or undefined type creation : use template specialization");
	}

protected:
	virtual std::shared_ptr<class VertexBuffer> createVertexBuffer_Impl(uint32_t vertexNum,
		const class Vertex* vertices) = 0;
public:
	template<>
	std::shared_ptr<class VertexBuffer> create<class VertexBuffer,
		uint32_t,
		const class Vertex*>(uint32_t vertexNum,
			const class Vertex* vertices);

protected:
	virtual std::shared_ptr<class ShaderModule> createShaderModule_Impl(class ILogicalDevice* device,
		size_t vsSize,
		size_t fsSize,
		char* vs,
		char* fs) = 0;
public:
	template<>
	std::shared_ptr<class ShaderModule> create<class ShaderModule,
		class ILogicalDevice*,
		size_t,
		size_t,
		char*,
		char*>(class ILogicalDevice* device,
			size_t vsSize,
			size_t fsSize,
			char* vs,
			char* fs);

protected:
	virtual std::shared_ptr<class CommandBuffer> createCommandBuffer_Impl() { return nullptr; }
public:
	template<>
	std::shared_ptr<class CommandBuffer> create<class CommandBuffer>();
};