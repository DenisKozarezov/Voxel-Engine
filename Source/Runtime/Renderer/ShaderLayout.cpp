#include "ShaderLayout.h"
#include <vector>

namespace VoxelEngine::renderer
{
	void ShaderLayout::calculateOffsetsAndStride()
	{
		size_t offset = 0;

		std::vector<ShaderLayoutElement*> nonInstanced, instanced;

		for (auto& element : m_elements)
		{
			if (element.m_instanced)
				instanced.push_back(&element);
			else
				nonInstanced.push_back(&element);
		}

		for (auto* element : nonInstanced)
		{
			element->m_offset = offset;
			offset += element->m_size;
			m_stride += element->m_size;
		}

		offset = 0;
		for (auto* element : instanced)
		{
			element->m_offset = offset;
			offset += element->m_size;
			m_stride += element->m_size;
		}
	}
	ShaderLayout::ShaderLayout(std::initializer_list<ShaderLayoutElement> elements)
		: m_elements(std::move(elements)), m_stride(0)
	{
		calculateOffsetsAndStride();
	}
	ShaderLayoutElement::ShaderLayoutElement(ShaderDataType type, bool instanced, bool normalized)
		: m_type(type), m_size(shaderDataTypeSize(type)), m_offset(0), m_instanced(instanced), m_normalized(normalized)
	{ }
}