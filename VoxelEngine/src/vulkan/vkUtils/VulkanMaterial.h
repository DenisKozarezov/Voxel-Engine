#pragma once
#include <vulkan/vulkan.h>
#include <components/mesh/Mesh.h>

namespace vkUtils
{
	struct VulkanMaterial : public VoxelEngine::components::mesh::Material
	{
		VkPipeline pipeline;
		VkPipelineLayout pipelineLayout;
		bool instanced = false;

		void bind(const VkCommandBuffer& commandBuffer)
		{
			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
		}
	};
}