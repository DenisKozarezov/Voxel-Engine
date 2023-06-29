#include "VulkanIndexBuffer.h"
#include "VulkanRenderer.h"

namespace VoxelEngine::renderer
{
	static SharedRef<VulkanRenderer> renderer = 0;

	IndexBuffer::IndexBuffer(const uint16* indices, const uint32& size, VkAllocationCallbacks* allocator)
		: _allocator(allocator)
	{
		renderer = VulkanRenderer::getInstance();
		_logicalDevice = renderer->getLogicalDevice();

		VkDeviceSize bufferSize = sizeof(indices[0]) * size;

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		renderer->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(_logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, indices, (size_t)bufferSize);
		vkUnmapMemory(_logicalDevice, stagingBufferMemory);

		renderer->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _indexBuffer, _indexBufferMemory);
		renderer->copyBuffer(stagingBuffer, _indexBuffer, bufferSize);

		vkDestroyBuffer(_logicalDevice, stagingBuffer, _allocator);
		vkFreeMemory(_logicalDevice, stagingBufferMemory, _allocator);
	}
	void IndexBuffer::bind() const
	{
		VkCommandBuffer commandBuffer = renderer->getCommandBuffer();
		vkCmdBindIndexBuffer(commandBuffer, _indexBuffer, 0, VK_INDEX_TYPE_UINT16);
	}
	void IndexBuffer::release() const
	{
		vkDestroyBuffer(_logicalDevice, _indexBuffer, _allocator);
		vkFreeMemory(_logicalDevice, _indexBufferMemory, _allocator);
	}
}