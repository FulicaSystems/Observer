#pragma once

#include <stdexcept>
#include <array>
#include <span>
#include <memory>

class ILowRenderer
{
protected:
	virtual void initGraphicsAPI_Impl(std::span<void*> args) = 0;


	// buffer object

	template<typename... TArgs>
	[[nodiscard]] std::shared_ptr<class IVertexBuffer> createBufferObject(uint32_t vertexNum,
		bool mappable = false,
		TArgs&&... args)
	{
		std::array<uint32_t, sizeof...(TArgs)> forwardedArgs = { std::forward<TArgs>(args)... };
		return createBufferObject_Impl(vertexNum, mappable, forwardedArgs);
	}
	[[nodiscard]] virtual std::shared_ptr<class IVertexBuffer> createBufferObject_Impl(uint32_t vertexNum,
		bool mappable,
		std::span<uint32_t> additionalArgs = std::span<uint32_t>()) = 0;
	virtual void populateBufferObject(class IVertexBuffer& vbo, const struct Vertex* vertices) = 0;
public:
	virtual void destroyBufferObject(class IVertexBuffer& vbo) = 0;


public:
	class Renderer* highRenderer = nullptr;

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

protected:
	virtual std::shared_ptr<class IVertexBuffer> createVertexBuffer_Impl(uint32_t vertexNum,
		const struct Vertex* vertices) = 0;
public:
	template<>
	std::shared_ptr<class IVertexBuffer> create<class IVertexBuffer,
		uint32_t,
		const struct Vertex*>(uint32_t vertexNum,
			const struct Vertex* vertices);

protected:
	virtual std::shared_ptr<class IShaderModule> createShaderModule_Impl(class ILogicalDevice* device,
		size_t vsSize,
		size_t fsSize,
		char* vs,
		char* fs) = 0;
	virtual void destroyShaderModule_Impl(std::shared_ptr<class IShaderModule> ptr) = 0;
public:
	template<>
	std::shared_ptr<class IShaderModule> create<class IShaderModule,
		class ILogicalDevice*,
		size_t,
		size_t,
		char*,
		char*>(class ILogicalDevice* device,
			size_t vsSize,
			size_t fsSize,
			char* vs,
			char* fs);
	template<>
	void destroy<class IShaderModule>(std::shared_ptr<class IShaderModule> ptr);

protected:
	virtual std::shared_ptr<class ILogicalDevice> createLogicalDevice_Impl() = 0;
	virtual void destroyLogicalDevice_Impl(std::shared_ptr<class ILogicalDevice> ptr) = 0;
public:
	template<>
	std::shared_ptr<class ILogicalDevice> create<class ILogicalDevice>();
	template<>
	void destroy<class ILogicalDevice>(std::shared_ptr<class ILogicalDevice> ptr);

protected:
	virtual std::shared_ptr<class CommandBuffer> createCommandBuffer_Impl() { return nullptr; }
public:
	template<>
	std::shared_ptr<class CommandBuffer> create<class CommandBuffer>();
};