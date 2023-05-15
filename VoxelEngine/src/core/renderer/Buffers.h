#pragma once
#include <pch.h>
#include <core/PrimitiveTypes.h>
#include <core/Base.h>

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


	//class BufferLayout
	//{
	//public:
	//	BufferLayout() {}

	//	BufferLayout(std::initializer_list<BufferElement> elements) : m_Elements(elements)
	//	{
	//		CalculateOffsetsAndStride();
	//	}

	//	uint32 GetStride() const { return m_Stride; }
	//	const std::vector<BufferElement>& GetElements() const { return m_Elements; }

	//	std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
	//	std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
	//	std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
	//	std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }
	//private:
	//	void CalculateOffsetsAndStride()
	//	{
	//		size_t offset = 0;
	//		m_Stride = 0;
	//		for (auto& element : m_Elements)
	//		{
	//			element.Offset = offset;
	//			offset += element.Size;
	//			m_Stride += element.Size;
	//		}
	//	}
	//private:
	//	std::vector<BufferElement> m_Elements;
	//	uint32 m_Stride = 0;
	//};

	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() = default;

		virtual void bind() const = 0;
		virtual void unbind() const = 0;

		virtual void setData(const void* data, const uint32& size) = 0;

		//virtual const BufferLayout& getLayout() const = 0;
		//virtual void setLayout(const BufferLayout& layout) = 0;

		static SharedRef<VertexBuffer> Create(const uint32& size);
		static SharedRef<VertexBuffer> Create(float* vertices, const uint32& size);
	};

	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() = default;

		virtual void bind() const = 0;
		virtual void unbind() const = 0;

		virtual const uint32& getCount() const = 0;

		static SharedRef<IndexBuffer> Create(uint32* indices, const uint32& count);
	};

}