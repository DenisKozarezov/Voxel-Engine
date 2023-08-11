#include "VulkanIndexBuffer.h"
#include "../VulkanBackend.h"

namespace vkUtils
{
	VulkanIndexBuffer::VulkanIndexBuffer(
		const VkPhysicalDevice& physicalDevice,
		const VkDevice& logicalDevice,
		const void* indices,
		const size_t& bufferSize) 
		: m_logicalDevice(logicalDevice)
	{
		auto stagingBuffer = memory::createBuffer(
			physicalDevice, 
			logicalDevice, 
			bufferSize, 
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

		stagingBuffer.map();
		stagingBuffer.setData(indices, bufferSize);
		stagingBuffer.unmap();

		m_indexBuffer = memory::createBuffer(
			physicalDevice,
			logicalDevice,
			bufferSize, 
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, 
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		vulkan::copyBuffer(stagingBuffer.buffer, m_indexBuffer.buffer, bufferSize);

		stagingBuffer.release();
	}
	void VulkanIndexBuffer::release() const
	{
		m_indexBuffer.release();
	}
}