#pragma once
#include <vector>
#include <vulkan/vulkan.h>
#include <Renderer/Renderer.h>

namespace vkUtils
{
	class VulkanQueryStatisticsPool
	{
	private:
		VkDevice m_logicalDevice = VK_NULL_HANDLE;
		VkQueryPool m_pool = VK_NULL_HANDLE;
		std::vector<uint64> m_statsData;
		RenderFrameStats m_frameStats{};
	public:
		VulkanQueryStatisticsPool(const VkDevice& logicalDevice);
		~VulkanQueryStatisticsPool();

		FORCE_INLINE const RenderFrameStats& getStats() const { return m_frameStats; } 
		
		void getQueryResults();
		void resetQuery(const VkCommandBuffer& commandBuffer);
		void beginQuery(const VkCommandBuffer& commandBuffer);
		void endQuery(const VkCommandBuffer& commandBuffer);
	};
}
