#include "VulkanVertexBuffer.h"
#include "vulkan/VulkanBackend.h"

namespace vkUtils
{
	VulkanVertexBuffer::VulkanVertexBuffer(const VkPhysicalDevice& physicalDevice, const VkDevice& logicalDevice, const void* vertices, const size_t& bufferSize)
	{ 
		setData(vertices, bufferSize);
	}

	void VulkanVertexBuffer::release() const
	{
		vertexBuffer.release(logicalDevice);
	}
	void VulkanVertexBuffer::setData(const void* data, const size_t& size)
	{
		const auto& stagingBuffer = vulkan::createBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		logicalDevice = vulkan::getLogicalDevice();
		vkUtils::memory::mapMemory(logicalDevice, stagingBuffer.bufferMemory, 0, size, 0, data);

		vertexBuffer = vulkan::createBuffer(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		vulkan::copyBuffer(stagingBuffer.buffer, vertexBuffer.buffer, size);

		vulkan::destroyBuffer(stagingBuffer.buffer);
		vulkan::freeDeviceMemory(stagingBuffer.bufferMemory);
	}

	VulkanVertexBuffer::~VulkanVertexBuffer()
	{
		release();
	}
}