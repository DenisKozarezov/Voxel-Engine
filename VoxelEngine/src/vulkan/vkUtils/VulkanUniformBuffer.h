#pragma once
#include <core/renderer/Buffers.h>
#include "VulkanAlloc.h"

namespace vkUtils
{
	class VulkanUniformBuffer : public VoxelEngine::renderer::UniformBuffer
	{
	private:
		VkDevice logicalDevice;
		memory::Buffer uniformBuffer;
		void* uniformBufferMapped;
	public:
		VulkanUniformBuffer() = default;
		VulkanUniformBuffer(const VkPhysicalDevice& physicalDevice, const VkDevice& logicalDevice, const uint32& size);

		operator const VkBuffer&() const & { return uniformBuffer.buffer; }

		void setData(const void* data, size_t size) const override;
		void release() const override;

		~VulkanUniformBuffer() = default;
	};
}