#pragma once
#include <pch.h>
#include <core/PrimitiveTypes.h>
#include <core/Base.h>

enum class ShaderDataType
{
	Undefined = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
};

constexpr static uint32 shaderDataTypeSize(ShaderDataType type)
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

namespace VoxelEngine::renderer
{
	struct ShaderLayoutElement
	{
		ShaderDataType m_type;
		uint32 m_size;
		size_t m_offset;
		bool m_instanced;
		bool m_normalized;

		ShaderLayoutElement() = default;
		ShaderLayoutElement(ShaderDataType type, bool instanced = false, bool normalized = false);
	};

	struct ShaderLayout
	{
	private:
		uint32 m_stride = 0;
		std::list<ShaderLayoutElement> m_elements;

		void calculateOffsetsAndStride();
	public:
		ShaderLayout(std::initializer_list<ShaderLayoutElement> elements);

		INLINE const uint32& stride() const noexcept { return m_stride; }
		INLINE size_t size() const noexcept { return m_elements.size(); }
		INLINE std::list<ShaderLayoutElement>::iterator begin() noexcept
		{
			return m_elements.begin();
		}
		INLINE std::list<ShaderLayoutElement>::iterator end() noexcept
		{
			return m_elements.end();
		}
		INLINE std::list<ShaderLayoutElement>::const_iterator cbegin() const noexcept
		{
			return m_elements.cbegin();
		}
		INLINE std::list<ShaderLayoutElement>::const_iterator cend() const noexcept
		{
			return m_elements.cend();
		}
	};
}