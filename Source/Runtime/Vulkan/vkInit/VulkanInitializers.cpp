#include "VulkanInitializers.h"
#include <Renderer/RenderingStructs.h>

namespace vkInit
{
	VkPipelineVertexInputStateCreateInfo inputStateCreateInfo(const VoxelEngine::renderer::ShaderLayout& layout, const uint32& vertexStride)
	{
		static std::vector<VkVertexInputAttributeDescription> shaderAttributes;
		static std::vector<VkVertexInputBindingDescription> shaderBindings;
		
		shaderAttributes.clear();
		shaderBindings.clear();
		shaderAttributes.reserve(layout.size());

		shaderBindings.push_back(vkInit::vertexInputBindingDescription(VERTEX_BUFFER_BIND_ID, vertexStride, VK_VERTEX_INPUT_RATE_VERTEX));
		
		uint32 location = 0;
		for (auto it = layout.cbegin(); it != layout.cend(); ++it, ++location)
		{
			const VkFormat format = vkInit::shaderDataTypeToVulkanBaseType(it->m_type);
			const uint32 binding = it->m_instanced ? INSTANCE_BUFFER_BIND_ID : VERTEX_BUFFER_BIND_ID;
			
			if (shaderBindings.size() < 2 && it->m_instanced)
			{
				shaderBindings.push_back(vkInit::vertexInputBindingDescription(
					binding,
					sizeof(InstanceData),
					VK_VERTEX_INPUT_RATE_INSTANCE));
			}			
			shaderAttributes.push_back(vkInit::vertexInputAttributeDescription(binding, location, format, it->m_offset));
		}

		const VkPipelineVertexInputStateCreateInfo vertexInputInfo = vkInit::pipelineVertexInputStateCreateInfo(
			shaderBindings.data(),
			static_cast<uint32>(shaderBindings.size()),
			shaderAttributes.data(),
			static_cast<uint32>(shaderAttributes.size()));
		return vertexInputInfo;
	}
}