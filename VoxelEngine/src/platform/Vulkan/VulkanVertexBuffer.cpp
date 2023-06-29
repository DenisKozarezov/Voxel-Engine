#include "VulkanVertexBuffer.h"
#include "VulkanRenderer.h"

namespace VoxelEngine::renderer
{
	static SharedRef<VulkanRenderer> renderer = 0;

	VertexBuffer::VertexBuffer(const Vertex* vertices, const uint32& size, VkAllocationCallbacks* allocator)
		: _allocator(allocator)
	{
		renderer = VulkanRenderer::getInstance();
		_logicalDevice = renderer->getLogicalDevice();

		VkDeviceSize bufferSize = sizeof(vertices[0]) * size;
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		renderer->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(_logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices, (size_t)bufferSize);
		vkUnmapMemory(_logicalDevice, stagingBufferMemory);

		renderer->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _vertexBuffer, _vertexBufferMemory);
		renderer->copyBuffer(stagingBuffer, _vertexBuffer, bufferSize);
		vkDestroyBuffer(_logicalDevice, stagingBuffer, _allocator);
		vkFreeMemory(_logicalDevice, stagingBufferMemory, _allocator);
	}

	void VertexBuffer::bind() const
	{
		VkCommandBuffer commandBuffer = renderer->getCommandBuffer();
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