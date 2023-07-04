#include "VulkanUniformBuffer.h"
#include "VulkanRenderer.h"

namespace VoxelEngine::renderer
{
	UniformBuffer::UniformBuffer(const VkDevice& logicalDevice, const uint32& size, VkAllocationCallbacks* allocator)
		: _logicalDevice(logicalDevice), _bufferSize(size)
	{
		auto renderer = VulkanRenderer::getInstance();
		renderer->createBuffer(size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, _uniformBuffer, _uniformBufferMemory);
		vkMapMemory(_logicalDevice, _uniformBufferMemory, 0, size, 0, &_uniformBufferMapped);
	}
	void UniformBuffer::setData(const void* data, size_t size) const
	{
		memcpy(_uniformBufferMapped, data, size);
	}
	void UniformBuffer::release() const
	{
		vkDestroyBuffer(_logicalDevice, _uniformBuffer, nullptr);
		vkFreeMemory(_logicalDevice, _uniformBufferMemory, nullptr);
	}
}