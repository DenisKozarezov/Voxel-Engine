#include "VulkanIndexBuffer.h"
#include "../VulkanBackend.h"

namespace vkUtils
{
	VulkanIndexBuffer::VulkanIndexBuffer(const VkPhysicalDevice& physicalDevice, const VkDevice& logicalDevice, const size_t& bufferSize)
		: m_physicalDevice(physicalDevice), m_logicalDevice(logicalDevice)
	{
		VOXEL_CORE_ASSERT(bufferSize > 0, "index buffer is attempting to allocate zero memory device size!");

		m_indexBuffer = memory::createBuffer(
			physicalDevice,
			logicalDevice,
			bufferSize,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		m_indexBuffer.map();
	}
	VulkanIndexBuffer::VulkanIndexBuffer(
		const VkPhysicalDevice& physicalDevice,
		const VkDevice& logicalDevice,
		const void* indices,
		const size_t& bufferSize) 
		: m_physicalDevice(physicalDevice), m_logicalDevice(logicalDevice)
	{
		VOXEL_CORE_ASSERT(indices, "index buffer is attempting to map empty data!");
		VOXEL_CORE_ASSERT(bufferSize > 0, "index buffer is attempting to allocate zero memory device size!");

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

		if (m_indexBuffer.size == 0)
		{
			this->m_indexBuffer = memory::createBuffer(
				buffer.m_physicalDevice,
				buffer.m_logicalDevice,
				buffer.m_indexBuffer.size,
				VK_BUFFER_USAGE_INDEX_BUFFER_BIT);

			this->m_logicalDevice = buffer.m_logicalDevice;
			this->m_physicalDevice = buffer.m_physicalDevice;
		}
		vulkan::copyBuffer(buffer.m_indexBuffer, this->m_indexBuffer, buffer.m_indexBuffer.size);
		this->m_indexBuffer.map();

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