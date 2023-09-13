#include "VulkanInitializers.h"
#include <core/renderer/RendererAPI.h>

namespace vkInit
{
	std::vector<VkVertexInputAttributeDescription> shaderAttributes;
	std::vector<VkVertexInputBindingDescription> shaderBindings;

	const VkPipelineVertexInputStateCreateInfo inputStateCreateInfo(VoxelEngine::renderer::ShaderLayout layout, const uint32& vertexStride)
	{
		shaderAttributes.clear();
		shaderBindings.clear();
		shaderAttributes.reserve(layout.size());

		uint32 i = 0;
		uint32 vertexRateStride = 0, instanceRateStride = 0;
		uint32 vertexRateSize = 0, instanceRateSize = 0;
		for (auto it = layout.cbegin(); it != layout.cend(); it++, ++i)
		{
			VkFormat format = vkInit::shaderDataTypeToVulkanBaseType(it->m_type);
			uint32 binding = it->m_instanced ? INSTANCE_BUFFER_BIND_ID : VERTEX_BUFFER_BIND_ID;

			if (it->m_instanced)
			{
				instanceRateStride += it->m_offset;
				instanceRateSize += it->m_size;
			}
			else
			{
				vertexRateStride += it->m_offset;
				vertexRateSize += it->m_size;
			}
			shaderAttributes.push_back(vkInit::vertexInputAttributeDescription(binding, i, format, it->m_offset));
		}

		shaderBindings =
		{
			vkInit::vertexInputBindingDescription(VERTEX_BUFFER_BIND_ID, vertexStride, VK_VERTEX_INPUT_RATE_VERTEX),
			vkInit::vertexInputBindingDescription(INSTANCE_BUFFER_BIND_ID, sizeof(VoxelEngine::renderer::InstanceData), VK_VERTEX_INPUT_RATE_INSTANCE)
		};

		VkPipelineVertexInputStateCreateInfo vertexInputInfo = vkInit::pipelineVertexInputStateCreateInfo(
			shaderBindings.data(),
			static_cast<uint32>(shaderBindings.size()),
			shaderAttributes.data(),
			static_cast<uint32>(shaderAttributes.size()));
		return vertexInputInfo;
	}
}