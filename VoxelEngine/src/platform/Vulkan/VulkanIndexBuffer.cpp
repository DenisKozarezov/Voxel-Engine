#include "VulkanIndexBuffer.h"
#include "VulkanRenderer.h"

namespace VoxelEngine::renderer
{
	IndexBuffer::IndexBuffer(const VkDevice& logicalDevice, const uint16* indices, const uint32& bufferSize, VkAllocationCallbacks* allocator)
		: _logicalDevice(logicalDevice), _allocator(allocator)
	{
		auto renderer = VulkanRenderer::getInstance();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		renderer->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, indices, (size_t)bufferSize);
		vkUnmapMemory(logicalDevice, stagingBufferMemory);

		renderer->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _indexBuffer, _indexBufferMemory);
		renderer->copyBuffer(stagingBuffer, _indexBuffer, bufferSize);

		vkDestroyBuffer(logicalDevice, stagingBuffer, _allocator);
		vkFreeMemory(logicalDevice, stagingBufferMemory, _allocator);
	}
	void IndexBuffer::bind(const VkCommandBuffer& commandBuffer) const
	{
		vkCmdBindIndexBuffer(commandBuffer, _indexBuffer, 0, VK_INDEX_TYPE_UINT16);
	}
	void IndexBuffer::release() const
	{
		vkDestroyBuffer(_logicalDevice, _indexBuffer, _allocator);
		vkFreeMemory(_logicalDevice, _indexBufferMemory, _allocator);
	}
}