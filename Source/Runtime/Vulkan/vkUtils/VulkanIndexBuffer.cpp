#include "VulkanIndexBuffer.h"
#include "../VulkanBackend.h"
#include <Core/Logging/Assert.h>

namespace vkUtils
{
	VulkanIndexBuffer::VulkanIndexBuffer(const vkInit::VulkanDevice& device, const size_t& bufferSize)
		: m_device(device)
	{
		VOXEL_CORE_ASSERT(bufferSize > 0, "index buffer is attempting to allocate zero memory device size!");

		m_indexBuffer = memory::createBuffer(
			device,
			bufferSize,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		m_indexBuffer.map();
	}
	VulkanIndexBuffer::VulkanIndexBuffer(const vkInit::VulkanDevice& device, const void* indices, const size_t& bufferSize) 
		: m_device(device)
	{
		VOXEL_CORE_ASSERT(indices, "index buffer is attempting to map empty data!");
		VOXEL_CORE_ASSERT(bufferSize > 0, "index buffer is attempting to allocate zero memory device size!");

		auto stagingBuffer = memory::createBuffer(
			device,
			bufferSize, 
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

		stagingBuffer.map();
		stagingBuffer.setData(indices, bufferSize);

		m_indexBuffer = memory::createBuffer(
			device,
			bufferSize, 
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, 
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		vulkan::copyBuffer(stagingBuffer, m_indexBuffer, bufferSize);
	}
	VulkanIndexBuffer::VulkanIndexBuffer(const VulkanIndexBuffer& rhs) : m_device(rhs.m_device)
	{
		this->m_indexBuffer = memory::createBuffer(
			rhs.m_device,
			rhs.m_indexBuffer.size,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		vulkan::copyBuffer(rhs.m_indexBuffer, this->m_indexBuffer, rhs.m_indexBuffer.size);

		this->m_indexBuffer.map();
	}
	VulkanIndexBuffer::VulkanIndexBuffer(VulkanIndexBuffer&& rhs) noexcept
		: m_device(std::move(rhs.m_device))
	{
		std::swap(this->m_indexBuffer, rhs.m_indexBuffer);
	}
	VulkanIndexBuffer& VulkanIndexBuffer::operator=(const VulkanIndexBuffer& rhs)
	{
		if (this == &rhs)
			return *this;

		release();

		this->m_device = rhs.m_device;

		if (this->m_indexBuffer.size == 0)
		{
			this->m_indexBuffer = memory::createBuffer(
				rhs.m_device,
				rhs.m_indexBuffer.size,
				VK_BUFFER_USAGE_INDEX_BUFFER_BIT);			
		}
		vulkan::copyBuffer(rhs.m_indexBuffer, this->m_indexBuffer, rhs.m_indexBuffer.size);
		this->m_indexBuffer.map();

		return *this;	
	}
	VulkanIndexBuffer& VulkanIndexBuffer::operator=(VulkanIndexBuffer&& rhs)
	{
		if (this == &rhs)
			return *this;

		release();
		std::swap(this->m_indexBuffer, rhs.m_indexBuffer);

		return *this;
	}
	VulkanIndexBuffer::~VulkanIndexBuffer()
	{
		release();
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
}