#include "VulkanIndexBuffer.h"
#include "../VulkanBackend.h"

namespace vkUtils
{
	VulkanIndexBuffer::VulkanIndexBuffer(
		const VkPhysicalDevice& physicalDevice,
		const VkDevice& logicalDevice,
		const void* indices,
		const size_t& bufferSize) 
		: logicalDevice(logicalDevice)
	{
		const auto& stagingBuffer = memory::createBuffer(
			physicalDevice, 
			logicalDevice, 
			bufferSize, 
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

		memory::mapMemory(logicalDevice, stagingBuffer.bufferMemory, 0, bufferSize, 0, indices);

		indexBuffer = memory::createBuffer(
			physicalDevice,
			logicalDevice,
			bufferSize, 
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, 
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		vulkan::copyBuffer(stagingBuffer.buffer, indexBuffer.buffer, bufferSize);

		memory::destroyBuffer(logicalDevice, stagingBuffer.buffer);
		memory::freeDeviceMemory(logicalDevice, stagingBuffer.bufferMemory);
	}
	void VulkanIndexBuffer::release() const
	{
		indexBuffer.release(logicalDevice);
	}
}