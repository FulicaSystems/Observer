#pragma once

#include <stdexcept>
#include <array>
#include <span>
#include <memory>

#include "graphicsapi.hpp"

class ILowRenderer
{
protected:
	virtual void initGraphicsAPI_Impl(std::span<void*> args) = 0;


public:
	EGraphicsAPI graphicsApi;

	ILowRenderer(const EGraphicsAPI graphicsApi = EGraphicsAPI::VULKAN) : graphicsApi(graphicsApi) {}

	template<typename... TArgs>
	void initGraphicsAPI(TArgs&&... args)
	{
		std::array<void*, sizeof...(TArgs)> forwardedArgs = { std::forward<TArgs>(args)... };
		initGraphicsAPI_Impl(forwardedArgs);
	}


	// graphics related object types

	template<typename TType, typename... TArgs>
	std::shared_ptr<TType> create(TArgs... args)
	{
		static_assert(sizeof(TType) == 0, "Invalid or undefined type creation : use template specialization");
	}
	template<typename TType>
	void destroy(std::shared_ptr<TType> ptr)
	{
		static_assert(sizeof(TType) == 0, "Invalid or undefined type destruction : use template specialization");
	}


	// vertex buffer
protected:
	template<typename... TArgs>
	[[nodiscard]] std::shared_ptr<class IVertexBuffer> createVertexBufferObject(uint32_t vertexNum,
		bool mappable = false,
		TArgs&&... args)
	{
		std::array<uint32_t, sizeof...(TArgs)> forwardedArgs = { std::forward<TArgs>(args)... };
		return createVertexBufferObject_Impl(vertexNum, mappable, forwardedArgs);
	}
	[[nodiscard]] virtual std::shared_ptr<class IVertexBuffer> createVertexBufferObject_Impl(uint32_t vertexNum,
		bool mappable,
		std::span<uint32_t> additionalArgs = std::span<uint32_t>()) = 0;
	virtual void populateVertexBufferObject(class IVertexBuffer& vbo, const struct Vertex* vertices) = 0;
protected:
	[[nodiscard]] virtual std::shared_ptr<class IVertexBuffer> createVertexBuffer_Impl(uint32_t vertexNum,
		const struct Vertex* vertices) = 0;
	virtual void destroyVertexBuffer_Impl(std::shared_ptr<class IVertexBuffer> ptr) = 0;
public:
	template<>
	[[nodiscard]] std::shared_ptr<class IVertexBuffer> create<class IVertexBuffer,
		uint32_t,
		const struct Vertex*>(uint32_t vertexNum,
			const struct Vertex* vertices);
	template<>
	void destroy<class IVertexBuffer>(std::shared_ptr<class IVertexBuffer> ptr);


	// shader module
protected:
	[[nodiscard]] virtual std::shared_ptr<class IShaderModule> createShaderModule_Impl(size_t vsSize,
		size_t fsSize,
		char* vs,
		char* fs) = 0;
	virtual void destroyShaderModule_Impl(std::shared_ptr<class IShaderModule> ptr) = 0;
public:
	template<>
	[[nodiscard]] std::shared_ptr<class IShaderModule> create<class IShaderModule,
		size_t,
		size_t,
		char*,
		char*>(size_t vsSize,
			size_t fsSize,
			char* vs,
			char* fs);
	template<>
	void destroy<class IShaderModule>(std::shared_ptr<class IShaderModule> ptr);
};