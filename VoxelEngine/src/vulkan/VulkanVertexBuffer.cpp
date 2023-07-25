#include "VulkanVertexBuffer.h"
#include "VulkanBackend.h"

namespace vulkan
{
	VulkanVertexBuffer::VulkanVertexBuffer(const void* vertices, const size_t& bufferSize)
	{ 
		setData(vertices, bufferSize);
	}

	void VulkanVertexBuffer::bind() const
	{
		VkBuffer vertexBuffers[] = { _vertexBuffer.buffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(vulkan::getCommandBuffer(), 0, 1, vertexBuffers, offsets);
	}
	void VulkanVertexBuffer::release() const
	{
		vulkan::destroyBuffer(_vertexBuffer.buffer);
		vulkan::freeDeviceMemory(_vertexBuffer.bufferMemory);
	}
	void VulkanVertexBuffer::setData(const void* data, const size_t& size)
	{
		const auto& stagingBuffer = vulkan::createBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		VkDevice logicalDevice = vulkan::getLogicalDevice();
		vkUtils::memory::mapMemory(logicalDevice, stagingBuffer.bufferMemory, 0, size, 0, data);

		_vertexBuffer = vulkan::createBuffer(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		vulkan::copyBuffer(stagingBuffer.buffer, _vertexBuffer.buffer, size);

		vulkan::destroyBuffer(stagingBuffer.buffer);
		vulkan::freeDeviceMemory(stagingBuffer.bufferMemory);
	}
}