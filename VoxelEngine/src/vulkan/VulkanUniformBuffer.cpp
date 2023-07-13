#include "VulkanUniformBuffer.h"
#include "VulkanBackend.h"

namespace vulkan
{
	VulkanUniformBuffer::VulkanUniformBuffer(const uint32& size)
		: UniformBuffer(size)
	{
		VkDevice logicalDevice = vulkan::getLogicalDevice();

		vulkan::createBuffer(size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, _uniformBuffer, _uniformBufferMemory);
		vkMapMemory(logicalDevice, _uniformBufferMemory, 0, size, 0, &_uniformBufferMapped);
	}
	void VulkanUniformBuffer::setData(const void* data, size_t size) const
	{
		memcpy(_uniformBufferMapped, data, size);
	}
	void VulkanUniformBuffer::release() const
	{
		vulkan::destroyBuffer(_uniformBuffer);
		vulkan::freeDeviceMemory(_uniformBufferMemory);
	}
}