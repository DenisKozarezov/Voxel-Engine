#pragma once
#include <vulkan/vulkan.h>
#include <Renderer/RenderSettings.h>

namespace vkUtils
{
	class VulkanQueryStatisticsPool
	{
	private:
		VkDevice m_logicalDevice = VK_NULL_HANDLE;
		VkQueryPool m_pool = VK_NULL_HANDLE;
		VoxelEngine::renderer::ShaderStats m_stats;
	public:
		VulkanQueryStatisticsPool(const VkDevice& logicalDevice);
		~VulkanQueryStatisticsPool();

		const VoxelEngine::renderer::ShaderStats& getStats() const { return m_stats; } 
		
		void getQueryResults();
		void resetQuery(const VkCommandBuffer& commandBuffer);
		void beginQuery(const VkCommandBuffer& commandBuffer);
		void endQuery(const VkCommandBuffer& commandBuffer);
	};
}
