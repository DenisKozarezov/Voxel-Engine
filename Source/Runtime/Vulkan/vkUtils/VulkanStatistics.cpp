#include "VulkanStatistics.h"
#include "VulkanValidation.h"

namespace vkUtils
{
    VulkanQueryStatisticsPool::VulkanQueryStatisticsPool(const VkDevice& logicalDevice)
        : m_logicalDevice(logicalDevice)
    {
        m_statsData.resize(6);

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
        m_statsData.clear();
        vkDestroyQueryPool(m_logicalDevice, m_pool, nullptr);
    }

    void VulkanQueryStatisticsPool::getQueryResults()
    {
        const uint32 dataSize = static_cast<uint32>(m_statsData.size()) * sizeof(uint64);
        const uint32 stride = static_cast<uint32>(m_statsData.size()) * sizeof(uint64);
        
        vkGetQueryPoolResults(
            m_logicalDevice,
            m_pool,
            0,
            1,
            dataSize,
            m_statsData.data(),
            stride,
            VK_QUERY_RESULT_64_BIT | VK_QUERY_RESULT_WAIT_BIT);

        m_frameStats.vertices = m_statsData[0];
        m_frameStats.primitives = m_statsData[1];
        m_frameStats.vertexShaderInvocations = m_statsData[2];
        m_frameStats.clippingInvocations = m_statsData[3];
        m_frameStats.clippingPrimitives = m_statsData[4];
        m_frameStats.fragmentShaderInvocations = m_statsData[5];
        m_frameStats.triangles = m_frameStats.vertices / 3;
    }

    void VulkanQueryStatisticsPool::resetQuery(const VkCommandBuffer& commandBuffer)
    {
        vkCmdResetQueryPool(commandBuffer, m_pool, 0, 1);
    }

    void VulkanQueryStatisticsPool::beginQuery(const VkCommandBuffer& commandBuffer)
    {
        vkCmdBeginQuery(commandBuffer, m_pool, 0, 0);
    }

    void VulkanQueryStatisticsPool::endQuery(const VkCommandBuffer& commandBuffer)
    {
        vkCmdEndQuery(commandBuffer, m_pool, 0);
    }
}
