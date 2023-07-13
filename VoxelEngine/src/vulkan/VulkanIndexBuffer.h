#pragma once
#include <core/renderer/Buffers.h>

namespace vulkan
{
	class VulkanIndexBuffer : public VoxelEngine::renderer::IndexBuffer
	{
	private:
		VkBuffer _indexBuffer;
		VkDeviceMemory _indexBufferMemory;
	public:
		VulkanIndexBuffer() = default;
		VulkanIndexBuffer(const void* indices, const size_t& bufferSize);

		operator const VkBuffer () const & { return _indexBuffer; }

		void bind() const override;
		void release() const override;
		void setData(const void* data, const size_t& size) override;

		~VulkanIndexBuffer() = default;
	};
}