#pragma once
#include <pch.h>
#include <core/PrimitiveTypes.h>
#include <core/Base.h>

enum class ShaderDataType
{
	Undefined = 0, 
	Float_S8, Float2_S8, Float3_S8, Float4_S8,
	Float_U8, Float2_U8, Float3_U8, Float4_U8,
	Float_S32, Float2_S32, Float3_S32, Float4_S32, 
	Float_U32, Float2_U32, Float3_U32, Float4_U32,
	Mat3, Mat4, 
	Int_S32, Int2_S32, Int3_S32, Int4_S32, 
	Bool
};

constexpr static uint32 shaderDataTypeSize(ShaderDataType type)
{
	switch (type)
	{
	case ShaderDataType::Float_S8:		return 1;
	case ShaderDataType::Float2_S8:		return 1 * 2;
	case ShaderDataType::Float3_S8:		return 1 * 3;
	case ShaderDataType::Float4_S8:		return 1 * 4;
	case ShaderDataType::Float_S32:		return 4;
	case ShaderDataType::Float2_S32:	return 4 * 2;
	case ShaderDataType::Float3_S32:	return 4 * 3;
	case ShaderDataType::Float4_S32:	return 4 * 4;
	case ShaderDataType::Float_U8:		return 1;
	case ShaderDataType::Float2_U8:		return 1 * 2;
	case ShaderDataType::Float3_U8:		return 1 * 3;
	case ShaderDataType::Float4_U8:		return 1 * 4;
	case ShaderDataType::Float_U32:		return 2;
	case ShaderDataType::Float2_U32:	return 2 * 2;
	case ShaderDataType::Float3_U32:	return 2 * 3;
	case ShaderDataType::Float4_U32:	return 2 * 4;
	case ShaderDataType::Mat3:			return 4 * 3 * 3;
	case ShaderDataType::Mat4:			return 4 * 4 * 4;
	case ShaderDataType::Int_S32:		return 4;
	case ShaderDataType::Int2_S32:		return 4 * 2;
	case ShaderDataType::Int3_S32:		return 4 * 3;
	case ShaderDataType::Int4_S32:		return 4 * 4;
	case ShaderDataType::Bool:			return 1;
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

		FORCE_INLINE const uint32& stride() const noexcept { return m_stride; }
		FORCE_INLINE size_t size() const noexcept { return m_elements.size(); }
		FORCE_INLINE std::list<ShaderLayoutElement>::iterator begin() noexcept
		{
			return m_elements.begin();
		}
		FORCE_INLINE std::list<ShaderLayoutElement>::iterator end() noexcept
		{
			return m_elements.end();
		}
		FORCE_INLINE std::list<ShaderLayoutElement>::const_iterator cbegin() const noexcept
		{
			return m_elements.cbegin();
		}
		FORCE_INLINE std::list<ShaderLayoutElement>::const_iterator cend() const noexcept
		{
			return m_elements.cend();
		}
	};
}