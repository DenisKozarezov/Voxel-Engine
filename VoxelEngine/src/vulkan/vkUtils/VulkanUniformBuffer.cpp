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
		m_uniformBuffer.map();
	}
	void VulkanUniformBuffer::setData(const void* data, const size_t& size) const
	{
		m_uniformBuffer.setData(data, size);
	}
	void VulkanUniformBuffer::release()
	{
		m_uniformBuffer.release();
	}
}