#include "ShaderLayout.h"
#include <algorithm>

namespace VoxelEngine::renderer
{
	void ShaderLayout::calculateOffsetsAndStride()
	{
		size_t instancedOffset = 0;
		size_t nonInstancedOffset = 0;
		
		auto&& pred = [&](ShaderLayoutElement& el)
		{
			if (el.m_instanced)
			{
				el.m_offset = instancedOffset;
				instancedOffset += el.m_size;
				m_stride += el.m_size;
			}
			else
			{
				el.m_offset = nonInstancedOffset;
				nonInstancedOffset += el.m_size;
				m_stride += el.m_size;
			}
		};
		std::for_each(m_elements.begin(), m_elements.end(), pred);
	}
	ShaderLayout::ShaderLayout(std::initializer_list<ShaderLayoutElement> elements)
		: m_stride(0), m_elements(elements)
	{
		calculateOffsetsAndStride();
	}
}