#include "VulkanVertexBuffer.h"
#include "VulkanBackend.h"
#include "VulkanAlloc.h"

namespace vulkan
{
	VulkanVertexBuffer::VulkanVertexBuffer(const void* vertices, const size_t& bufferSize)
	{ 
		setData(vertices, bufferSize);
	}

	void VulkanVertexBuffer::bind() const
	{
		VkBuffer vertexBuffers[] = { _vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(vulkan::getCommandBuffer(), 0, 1, vertexBuffers, offsets);
	}
	void VulkanVertexBuffer::release() const
	{
		vulkan::destroyBuffer(_vertexBuffer);
		vulkan::freeDeviceMemory(_vertexBufferMemory);
	}
	void VulkanVertexBuffer::setData(const void* data, const size_t& size)
	{
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		vulkan::createBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		VkDevice logicalDevice = vulkan::getLogicalDevice();
		memory::mapMemory(logicalDevice, stagingBufferMemory, 0, size, 0, data);

		vulkan::createBuffer(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _vertexBuffer, _vertexBufferMemory);
		vulkan::copyBuffer(stagingBuffer, _vertexBuffer, size);

		vulkan::destroyBuffer(stagingBuffer);
		vulkan::freeDeviceMemory(stagingBufferMemory);
	}
}