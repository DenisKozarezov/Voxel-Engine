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
		vkCmdBindIndexBuffer(vulkan::getCommandBuffer(), _indexBuffer, 0, VK_INDEX_TYPE_UINT16);
	}
	void VulkanIndexBuffer::release() const
	{
		vulkan::destroyBuffer(_indexBuffer);
		vulkan::freeDeviceMemory(_indexBufferMemory);
	}
	void VulkanIndexBuffer::setData(const void* data, const size_t& size)
	{
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		vulkan::createBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		VkDevice logicalDevice = vulkan::getLogicalDevice();

		void* dst;
		vkMapMemory(logicalDevice, stagingBufferMemory, 0, size, 0, &dst);
		memcpy(dst, data, size);
		vkUnmapMemory(logicalDevice, stagingBufferMemory);

		vulkan::createBuffer(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _indexBuffer, _indexBufferMemory);
		vulkan::copyBuffer(stagingBuffer, _indexBuffer, size);

		vulkan::destroyBuffer(stagingBuffer);
		vulkan::freeDeviceMemory(stagingBufferMemory);
	}
}