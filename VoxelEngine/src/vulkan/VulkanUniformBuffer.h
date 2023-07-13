#pragma once
#include <vulkan/vulkan.h>
#include <core/renderer/Buffers.h>

namespace vulkan
{
	class VulkanUniformBuffer : public VoxelEngine::renderer::UniformBuffer
	{
	private:
		VkBuffer _uniformBuffer;
		VkDeviceMemory _uniformBufferMemory;
		void* _uniformBufferMapped;
	public:
		VulkanUniformBuffer() = default;
		VulkanUniformBuffer(const uint32& size);

		operator VkBuffer() const & { return _uniformBuffer; }

		void setData(const void* data, size_t size) const override;
		void release() const override;

		~VulkanUniformBuffer() = default;
	};
}