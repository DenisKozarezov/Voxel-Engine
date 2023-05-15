#pragma once
#include "VulkanRenderer.h"

namespace VoxelEngine::renderer
{
	class VertexBuffer
	{
	private:
		VkBuffer _vertexBuffer;
		VkDeviceMemory _vertexBufferMemory;
		VkAllocationCallbacks* _allocator;
		VkDevice _logicalDevice;
	public:
		VertexBuffer(const Vertex* vertices, const uint32& size, VkAllocationCallbacks* allocator = nullptr)
			: _allocator(allocator)
		{
			_logicalDevice = VulkanRenderer::getLogicalDevice();

			VkDeviceSize bufferSize = sizeof(vertices[0]) * size;
			VkBuffer stagingBuffer;
			VkDeviceMemory stagingBufferMemory;
			VulkanRenderer::createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

			void* data;
			vkMapMemory(_logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
			std::copy(vertices, vertices + (size_t)bufferSize, data);
			vkUnmapMemory(_logicalDevice, stagingBufferMemory);

			VulkanRenderer::createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _vertexBuffer, _vertexBufferMemory);
			VulkanRenderer::copyBuffer(stagingBuffer, _vertexBuffer, bufferSize);
			vkDestroyBuffer(_logicalDevice, stagingBuffer, _allocator);
			vkFreeMemory(_logicalDevice, stagingBufferMemory, _allocator);
		}

		~VertexBuffer()
		{
			vkDestroyBuffer(_logicalDevice, _vertexBuffer, _allocator);
			vkFreeMemory(_logicalDevice, _vertexBufferMemory, _allocator);
		}
	};
}