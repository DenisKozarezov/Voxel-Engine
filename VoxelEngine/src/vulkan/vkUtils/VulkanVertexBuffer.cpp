#include "VulkanVertexBuffer.h"
#include "vulkan/VulkanBackend.h"

namespace vkUtils
{
	VulkanVertexBuffer::VulkanVertexBuffer(const VkPhysicalDevice& physicalDevice, const VkDevice& logicalDevice, const size_t& bufferSize)
		: m_physicalDevice(physicalDevice), m_logicalDevice(logicalDevice)
	{
		VOXEL_CORE_ASSERT(bufferSize > 0, "vertex buffer is attempting to allocate zero memory device size!");

		m_vertexBuffer = memory::createBuffer(
			physicalDevice,
			logicalDevice,
			bufferSize,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
		m_vertexBuffer.map();
	}
	VulkanVertexBuffer::VulkanVertexBuffer(
		const VkPhysicalDevice& physicalDevice, 
		const VkDevice& logicalDevice, 
		const void* vertices, 
		const size_t& bufferSize)
		: m_physicalDevice(physicalDevice), m_logicalDevice(logicalDevice)
	{ 
		VOXEL_CORE_ASSERT(vertices, "vertex buffer is attempting to map empty data!");
		VOXEL_CORE_ASSERT(bufferSize > 0, "vertex buffer is attempting to allocate zero memory device size!");

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

		if (m_vertexBuffer.size == 0)
		{
			this->m_vertexBuffer = memory::createBuffer(
				buffer.m_physicalDevice,
				buffer.m_logicalDevice,
				buffer.m_vertexBuffer.size,
				VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

			this->m_logicalDevice = buffer.m_logicalDevice;
			this->m_physicalDevice = buffer.m_physicalDevice;
		}
		vulkan::copyBuffer(buffer.m_vertexBuffer, this->m_vertexBuffer, buffer.m_vertexBuffer.size);
		this->m_vertexBuffer.map();

		return *this;
	}

	constexpr uint32 VulkanVertexBuffer::size() const
	{
		return static_cast<uint32>(m_vertexBuffer.size);
	}

	void VulkanVertexBuffer::setData(const void* data, const uint32& size)
	{
		m_vertexBuffer.setData(data, size);
	}
	void VulkanVertexBuffer::bind(const uint32& binding)
	{
		VkCommandBuffer commandBuffer = vulkan::getCommandBuffer();
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, binding, 1, &m_vertexBuffer.buffer, offsets);
	}
	void VulkanVertexBuffer::bind(const VkCommandBuffer& commandBuffer, const uint32& binding)
	{
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