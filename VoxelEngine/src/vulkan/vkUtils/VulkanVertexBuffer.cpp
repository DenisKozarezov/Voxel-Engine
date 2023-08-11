#include "VulkanVertexBuffer.h"
#include "vulkan/VulkanBackend.h"

namespace vkUtils
{
	VulkanVertexBuffer::VulkanVertexBuffer(
		const VkPhysicalDevice& physicalDevice, 
		const VkDevice& logicalDevice, 
		const void* vertices, 
		const size_t& bufferSize)
		: m_logicalDevice(logicalDevice)
	{ 
		const auto& stagingBuffer = memory::createBuffer(
			physicalDevice,
			logicalDevice,
			bufferSize, 
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

		memory::mapMemory(logicalDevice, stagingBuffer.bufferMemory, 0, bufferSize, 0, vertices);

		m_vertexBuffer = memory::createBuffer(
			physicalDevice,
			logicalDevice,
			bufferSize, 
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		vulkan::copyBuffer(stagingBuffer.buffer, m_vertexBuffer.buffer, bufferSize);

		stagingBuffer.release();
	}

	void VulkanVertexBuffer::release() const
	{
		m_vertexBuffer.release();
	}

	VulkanVertexBuffer::~VulkanVertexBuffer()
	{
		release();
	}
}