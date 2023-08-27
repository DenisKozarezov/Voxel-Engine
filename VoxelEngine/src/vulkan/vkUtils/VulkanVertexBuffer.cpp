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
		auto stagingBuffer = memory::createBuffer(
			physicalDevice,
			logicalDevice,
			bufferSize, 
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

		stagingBuffer.map();
		stagingBuffer.setData(vertices, bufferSize);
		stagingBuffer.unmap();

		m_vertexBuffer = memory::createBuffer(
			physicalDevice,
			logicalDevice,
			bufferSize, 
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		vulkan::copyBuffer(stagingBuffer, m_vertexBuffer, bufferSize);

		stagingBuffer.release();
	}

	VulkanVertexBuffer& VulkanVertexBuffer::operator=(const VulkanVertexBuffer& buffer)
	{
		this->m_vertexBuffer.unmap();
		this->m_vertexBuffer.release();
		vulkan::copyBuffer(buffer.m_vertexBuffer, this->m_vertexBuffer, buffer.m_vertexBuffer.size);
		return *this;
	}

	constexpr uint32 VulkanVertexBuffer::size() const
	{
		return static_cast<uint32>(m_vertexBuffer.size);
	}

	void VulkanVertexBuffer::setData(const void* data, const uint32& size)
	{
		
	}
	void VulkanVertexBuffer::bind(const uint32& binding)
	{
		VkCommandBuffer commandBuffer = vulkan::getCommandBuffer();
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, binding, 1, &m_vertexBuffer.buffer, offsets);
	}
	void VulkanVertexBuffer::release()
	{
		m_vertexBuffer.release();
	}

	VulkanVertexBuffer::~VulkanVertexBuffer()
	{
		release();
	}
}