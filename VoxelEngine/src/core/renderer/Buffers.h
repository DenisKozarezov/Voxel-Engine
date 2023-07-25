#pragma once
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <pch.h>
#include <glm/mat4x4.hpp>
#include <vulkan/VulkanVertex.h>
#include <core/PrimitiveTypes.h>

namespace VoxelEngine::renderer
{
	enum class ShaderDataType : byte
	{
		None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
	};

	static const uint32 ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Float:    return 4;
		case ShaderDataType::Float2:   return 4 * 2;
		case ShaderDataType::Float3:   return 4 * 3;
		case ShaderDataType::Float4:   return 4 * 4;
		case ShaderDataType::Mat3:     return 4 * 3 * 3;
		case ShaderDataType::Mat4:     return 4 * 4 * 4;
		case ShaderDataType::Int:      return 4;
		case ShaderDataType::Int2:     return 4 * 2;
		case ShaderDataType::Int3:     return 4 * 3;
		case ShaderDataType::Int4:     return 4 * 4;
		case ShaderDataType::Bool:     return 1;
		default: return 0;
		}
	}

	//struct BufferElement
	//{
	//	string Name;
	//	ShaderDataType Type;
	//	uint32_t Size;
	//	size_t Offset;
	//	bool Normalized;

	//	BufferElement() = default;

	//	BufferElement(ShaderDataType type, const string& name, bool normalized = false)
	//		: Name(name), Type(type), Size(ShaderDataTypeSize(type)), Offset(0), Normalized(normalized)
	//	{
	//	}

	//	uint32_t GetComponentCount() const
	//	{
	//		switch (Type)
	//		{
	//		case ShaderDataType::Float:   return 1;
	//		case ShaderDataType::Float2:  return 2;
	//		case ShaderDataType::Float3:  return 3;
	//		case ShaderDataType::Float4:  return 4;
	//		case ShaderDataType::Mat3:    return 3; // 3* float3
	//		case ShaderDataType::Mat4:    return 4; // 4* float4
	//		case ShaderDataType::Int:     return 1;
	//		case ShaderDataType::Int2:    return 2;
	//		case ShaderDataType::Int3:    return 3;
	//		case ShaderDataType::Int4:    return 4;
	//		case ShaderDataType::Bool:    return 1;
	//		}
	//		return 0;
	//	}
	//};
	

	class VertexBuffer
	{
	public:
		VertexBuffer() = default;

		virtual void bind() const = 0;
		virtual void release() const = 0;
		virtual void setData(const void* data, const size_t& size) = 0;

		virtual ~VertexBuffer() = default;

		static VertexBuffer* Create(const void* data, const size_t& size);
	};

	class IndexBuffer
	{
	public:
		IndexBuffer() = default;

		virtual void bind() const = 0;
		virtual void release() const = 0;
		virtual void setData(const void* data, const size_t& size) = 0;

		virtual ~IndexBuffer() = default;

		static IndexBuffer* Create(const void* data, const size_t& size);
	};	

	class UniformBuffer
	{
	private:
		uint32 _bufferSize;
	public:
		UniformBuffer() = default;
		UniformBuffer(const uint32& size) : _bufferSize(size) { }

		virtual void setData(const void* data, size_t size) const = 0;
		virtual void release() const = 0;

		static UniformBuffer* Create(const size_t& size);

		~UniformBuffer() = default;
	};
}