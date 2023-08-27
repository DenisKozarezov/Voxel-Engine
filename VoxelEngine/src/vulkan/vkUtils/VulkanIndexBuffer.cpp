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
		vulkan::copyBuffer(stagingBuffer, m_indexBuffer, bufferSize);

		stagingBuffer.release();
	}
	VulkanIndexBuffer& VulkanIndexBuffer::operator=(const VulkanIndexBuffer& buffer)
	{
		this->m_indexBuffer.unmap();
		this->m_indexBuffer.release();
		vulkan::copyBuffer(buffer.m_indexBuffer, this->m_indexBuffer, buffer.m_indexBuffer.size);
		return *this;
	}
	constexpr uint32 VulkanIndexBuffer::size() const
	{
		return static_cast<uint32>(m_indexBuffer.size);
	}
	void VulkanIndexBuffer::bind()
	{
		VkCommandBuffer commandBuffer = vulkan::getCommandBuffer();
		vkCmdBindIndexBuffer(commandBuffer, m_indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);
	}
	void VulkanIndexBuffer::release()
	{
		m_indexBuffer.release();
	}
}