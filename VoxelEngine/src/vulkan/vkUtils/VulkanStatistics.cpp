#include "VulkanStatistics.h"
#include "VulkanValidation.h"

namespace vkUtils
{
    VulkanQueryStatisticsPool::VulkanQueryStatisticsPool(const VkDevice& logicalDevice)
        : m_logicalDevice(logicalDevice)
    {
        std::vector<string> pipelineStatNames =
        {
            "Input assembly vertex count        ",
            "Input assembly primitives count    ",
            "Vertex shader invocations          ",
            "Clipping stage primitives processed",
            "Clipping stage primitives output   ",
            "Fragment shader invocations        "
        };
        
        m_stats.performanceStats.resize(pipelineStatNames.size());
        m_stats.performanceStrings = std::vector(pipelineStatNames.begin(), pipelineStatNames.end());
        
        VkQueryPoolCreateInfo queryPoolInfo{};
        queryPoolInfo.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
        queryPoolInfo.queryType = VK_QUERY_TYPE_PIPELINE_STATISTICS;

        // Pipeline counters to be returned for this pool
        queryPoolInfo.pipelineStatistics =
            VK_QUERY_PIPELINE_STATISTIC_INPUT_ASSEMBLY_VERTICES_BIT |
            VK_QUERY_PIPELINE_STATISTIC_INPUT_ASSEMBLY_PRIMITIVES_BIT |
            VK_QUERY_PIPELINE_STATISTIC_VERTEX_SHADER_INVOCATIONS_BIT |
            VK_QUERY_PIPELINE_STATISTIC_CLIPPING_INVOCATIONS_BIT |
            VK_QUERY_PIPELINE_STATISTIC_CLIPPING_PRIMITIVES_BIT |
            VK_QUERY_PIPELINE_STATISTIC_FRAGMENT_SHADER_INVOCATIONS_BIT;

        queryPoolInfo.queryCount = 1;
        
        VkResult err = vkCreateQueryPool(logicalDevice, &queryPoolInfo, NULL, &m_pool);
        VK_CHECK(err, "failed to create statistics query pool!");
    }

    VulkanQueryStatisticsPool::~VulkanQueryStatisticsPool()
    {
        vkDestroyQueryPool(m_logicalDevice, m_pool, nullptr);
    }

    void VulkanQueryStatisticsPool::getQueryResults()
    {
        const uint32 dataSize = static_cast<uint32>(m_stats.performanceStats.size()) * sizeof(uint64);
        const uint32 stride = static_cast<uint32>(m_stats.performanceStrings.size()) * sizeof(uint64);
        
        vkGetQueryPoolResults(
            m_logicalDevice,
            m_pool,
            0,
            1,
            dataSize,
            m_stats.performanceStats.data(),
            stride,
            VK_QUERY_RESULT_64_BIT);
    }

    void VulkanQueryStatisticsPool::beginQuery(const VkCommandBuffer& commandBuffer)
    {
        vkCmdResetQueryPool(commandBuffer, m_pool, 0, 1);
        vkCmdBeginQuery(commandBuffer, m_pool, 0, 0);
    }

    void VulkanQueryStatisticsPool::endQuery(const VkCommandBuffer& commandBuffer)
    {
        vkCmdEndQuery(commandBuffer, m_pool, 0);
    }
}
