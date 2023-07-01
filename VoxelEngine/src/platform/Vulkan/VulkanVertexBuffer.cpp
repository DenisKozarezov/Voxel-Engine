#include "VulkanVertexBuffer.h"
#include "VulkanRenderer.h"

namespace VoxelEngine::renderer
{
	VertexBuffer::VertexBuffer(const VkDevice& logicalDevice, const Vertex* vertices, const uint32& bufferSize, VkAllocationCallbacks* allocator)
		: _logicalDevice(logicalDevice), _allocator(allocator)
	{
		auto renderer = VulkanRenderer::getInstance();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		renderer->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices, (size_t)bufferSize);
		vkUnmapMemory(logicalDevice, stagingBufferMemory);

		renderer->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _vertexBuffer, _vertexBufferMemory);
		renderer->copyBuffer(stagingBuffer, _vertexBuffer, bufferSize);
		vkDestroyBuffer(logicalDevice, stagingBuffer, _allocator);
		vkFreeMemory(logicalDevice, stagingBufferMemory, _allocator);
	}

	void VertexBuffer::bind(const VkCommandBuffer& commandBuffer) const
	{
		VkBuffer vertexBuffers[] = { _vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
	}
	void VertexBuffer::release() const
	{
		vkDestroyBuffer(_logicalDevice, _vertexBuffer, _allocator);
		vkFreeMemory(_logicalDevice, _vertexBufferMemory, _allocator);
	}
}