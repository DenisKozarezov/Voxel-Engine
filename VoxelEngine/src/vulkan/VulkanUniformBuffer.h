#pragma once
#include <vulkan/VulkanAlloc.h>
#include <core/renderer/Buffers.h>

namespace vulkan
{
	class VulkanUniformBuffer : public VoxelEngine::renderer::UniformBuffer
	{
	private:
		memory::Buffer _uniformBuffer;
		void* _uniformBufferMapped;
	public:
		VulkanUniformBuffer() = default;
		VulkanUniformBuffer(const uint32& size);

		operator VkBuffer() const & { return _uniformBuffer.buffer; }

		void setData(const void* data, size_t size) const override;
		void release() const override;

		~VulkanUniformBuffer() = default;
	};
}