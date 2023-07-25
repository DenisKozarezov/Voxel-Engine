#include "VulkanUniformBuffer.h"

namespace vkUtils
{
	VulkanUniformBuffer::VulkanUniformBuffer(const VkPhysicalDevice& physicalDevice, const VkDevice& logicalDevice, const uint32& size)
		: UniformBuffer(size), _logicalDevice(logicalDevice)
	{
		_uniformBuffer = memory::createBuffer(
			physicalDevice, 
			logicalDevice, 
			size, 
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

		vkMapMemory(logicalDevice, _uniformBuffer.bufferMemory, 0, size, 0, &_uniformBufferMapped);
	}
	void VulkanUniformBuffer::setData(const void* data, size_t size) const
	{
		memcpy(_uniformBufferMapped, data, size);
	}
	void VulkanUniformBuffer::release() const
	{
		memory::destroyBuffer(_logicalDevice, _uniformBuffer.buffer);
		memory::freeDeviceMemory(_logicalDevice, _uniformBuffer.bufferMemory);
	}
}