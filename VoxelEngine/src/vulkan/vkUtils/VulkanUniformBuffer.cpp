#include "VulkanUniformBuffer.h"

namespace vkUtils
{
	VulkanUniformBuffer::VulkanUniformBuffer(
		const VkPhysicalDevice& physicalDevice, 
		const VkDevice& logicalDevice, 
		const uint32& size)
		: logicalDevice(logicalDevice)
	{
		uniformBuffer = memory::createBuffer(
			physicalDevice, 
			logicalDevice, 
			size, 
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

		vkMapMemory(logicalDevice, uniformBuffer.bufferMemory, 0, size, 0, &uniformBufferMapped);
	}
	void VulkanUniformBuffer::setData(const void* data, size_t size) const
	{
		memcpy(uniformBufferMapped, data, size);
	}
	void VulkanUniformBuffer::release() const
	{
		uniformBuffer.release();
	}
}