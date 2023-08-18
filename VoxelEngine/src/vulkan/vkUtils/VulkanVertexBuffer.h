#pragma once
#include <core/renderer/Buffer.h>
#include <vulkan/vkUtils/VulkanAlloc.h>

namespace vkUtils
{
	class VulkanVertexBuffer : public VoxelEngine::renderer::VertexBuffer
	{
	private:
		VkDevice m_logicalDevice;
		vkUtils::memory::Buffer m_vertexBuffer;
	public:
		VulkanVertexBuffer() noexcept = default;
		VulkanVertexBuffer(
			const VkPhysicalDevice& physicalDevice, 
			const VkDevice& logicalDevice, 
			const void* vertices, 
			const size_t& bufferSize);
		VulkanVertexBuffer& operator=(const VulkanVertexBuffer& buffer);

		inline operator const VkBuffer&() const & { return m_vertexBuffer.buffer; }

		constexpr uint32 size() const override;

		void setData(const void* data, const uint32& size) override;
		void release() override;

		~VulkanVertexBuffer();
	};	
}