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

		m_vertexBuffer = memory::createBuffer(
			physicalDevice,
			logicalDevice,
			bufferSize, 
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		vulkan::copyBuffer(stagingBuffer, m_vertexBuffer, bufferSize);
	}

	VulkanVertexBuffer::VulkanVertexBuffer(const VulkanVertexBuffer& rhs) :
		m_physicalDevice(rhs.m_physicalDevice), m_logicalDevice(rhs.m_logicalDevice)
	{
		this->m_vertexBuffer = memory::createBuffer(
			rhs.m_physicalDevice,
			rhs.m_logicalDevice,
			rhs.m_vertexBuffer.size,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

		vulkan::copyBuffer(rhs.m_vertexBuffer, this->m_vertexBuffer, rhs.m_vertexBuffer.size);

		this->m_vertexBuffer.map();
	}
	VulkanVertexBuffer::VulkanVertexBuffer(VulkanVertexBuffer&& rhs) noexcept 
		: m_physicalDevice(std::move(rhs.m_physicalDevice)), m_logicalDevice(std::move(rhs.m_logicalDevice))
	{
		std::swap(this->m_vertexBuffer, rhs.m_vertexBuffer);
	}
	VulkanVertexBuffer& VulkanVertexBuffer::operator=(const VulkanVertexBuffer& rhs)
	{
		if (this == &rhs)
			return *this;

		release();

		this->m_logicalDevice = rhs.m_logicalDevice;
		this->m_physicalDevice = rhs.m_physicalDevice;

		if (this->m_vertexBuffer.size == 0)
		{
			this->m_vertexBuffer = memory::createBuffer(
				rhs.m_physicalDevice,
				rhs.m_logicalDevice,
				rhs.m_vertexBuffer.size,
				VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
		}
		vulkan::copyBuffer(rhs.m_vertexBuffer, this->m_vertexBuffer, rhs.m_vertexBuffer.size);
		this->m_vertexBuffer.map();
		return *this;
	}
	VulkanVertexBuffer& VulkanVertexBuffer::operator=(VulkanVertexBuffer&& rhs) noexcept
	{
		if (this == &rhs)
			return *this;

		release();
		std::swap(this->m_vertexBuffer, rhs.m_vertexBuffer);

		return *this;
	}

	VulkanVertexBuffer::~VulkanVertexBuffer()
	{
		release();
	}

	constexpr uint32 VulkanVertexBuffer::size() const
	{
		return static_cast<uint32>(m_vertexBuffer.size);
	}

	INLINE void VulkanVertexBuffer::setData(const void* data, const uint32& size)
	{
		m_vertexBuffer.setData(data, size);
	}
	void VulkanVertexBuffer::bind(const uint32& binding)
	{
		VkCommandBuffer commandBuffer = vulkan::getCommandBuffer();
		bind(commandBuffer, binding);
	}
	void VulkanVertexBuffer::bind(const VkCommandBuffer& commandBuffer, const uint32& binding)
	{
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, binding, 1, &m_vertexBuffer.buffer, offsets);
	}
	INLINE void VulkanVertexBuffer::release()
	{
		m_vertexBuffer.release();
	}
}