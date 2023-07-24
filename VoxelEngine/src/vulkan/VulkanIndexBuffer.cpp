#include "VulkanIndexBuffer.h"
#include "VulkanBackend.h"

namespace vulkan
{
	VulkanIndexBuffer::VulkanIndexBuffer(const void* indices, const size_t& bufferSize)
	{
		setData(indices, bufferSize);
	}
	void VulkanIndexBuffer::bind() const
	{
		vkCmdBindIndexBuffer(vulkan::getCommandBuffer(), _indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT16);
	}
	void VulkanIndexBuffer::release() const
	{
		vulkan::destroyBuffer(_indexBuffer.buffer);
		vulkan::freeDeviceMemory(_indexBuffer.bufferMemory);
	}
	void VulkanIndexBuffer::setData(const void* data, const size_t& size)
	{
		const auto& stagingBuffer = vulkan::createBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		VkDevice logicalDevice = vulkan::getLogicalDevice();
		memory::mapMemory(logicalDevice, stagingBuffer.bufferMemory, 0, size, 0, data);

		_indexBuffer = vulkan::createBuffer(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		vulkan::copyBuffer(stagingBuffer.buffer, _indexBuffer.buffer, size);

		vulkan::destroyBuffer(stagingBuffer.buffer);
		vulkan::freeDeviceMemory(stagingBuffer.bufferMemory);
	}
}