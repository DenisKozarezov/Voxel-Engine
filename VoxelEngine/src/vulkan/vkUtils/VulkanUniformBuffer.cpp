#include "VulkanUniformBuffer.h"

namespace vkUtils
{
	VulkanUniformBuffer::VulkanUniformBuffer(
		const VkPhysicalDevice& physicalDevice, 
		const VkDevice& logicalDevice, 
		const uint32& size)
		: m_logicalDevice(logicalDevice)
	{
		m_uniformBuffer = memory::createBuffer(
			physicalDevice, 
			logicalDevice, 
			size, 
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

		vkMapMemory(logicalDevice, m_uniformBuffer.bufferMemory, 0, size, 0, &m_uniformBufferMapped);
	}
	void VulkanUniformBuffer::setData(const void* data, size_t size) const
	{
		memcpy(m_uniformBufferMapped, data, size);
	}
	void VulkanUniformBuffer::release() const
	{
		m_uniformBuffer.release();
	}
}