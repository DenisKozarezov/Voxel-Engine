#include "VulkanUniformBuffer.h"
#include "VulkanRenderer.h"

namespace VoxelEngine::renderer
{
	UniformBuffer::UniformBuffer(const uint32& size, VkAllocationCallbacks* allocator)
		: _allocator(allocator)
	{
		auto renderer = VulkanRenderer::getInstance();
		_logicalDevice = renderer->getLogicalDevice();
		renderer->createBuffer(size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, _uniformBuffer, _uniformBufferMemory);
	}
	void UniformBuffer::bind()
	{

	}
	void UniformBuffer::unbind()
	{
		vkDestroyBuffer(_logicalDevice, _uniformBuffer, _allocator);
		vkFreeMemory(_logicalDevice, _uniformBufferMemory, _allocator);
	}
}