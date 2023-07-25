#pragma once
#include <core/renderer/Buffers.h>
#include "VulkanAlloc.h"

namespace vkUtils
{
	class VulkanUniformBuffer : public VoxelEngine::renderer::UniformBuffer
	{
	private:
		VkDevice _logicalDevice;
		memory::Buffer _uniformBuffer;
		void* _uniformBufferMapped;
	public:
		VulkanUniformBuffer() = default;
		VulkanUniformBuffer(const VkPhysicalDevice& physicalDevice, const VkDevice& logicalDevice, const uint32& size);

		operator VkBuffer() const & { return _uniformBuffer.buffer; }

		void setData(const void* data, size_t size) const override;
		void release() const override;

		~VulkanUniformBuffer() = default;
	};
}