#pragma once
#include <core/PrimitiveTypes.h>
#include "VulkanVertex.h"

namespace VoxelEngine::renderer
{
	class IndexBuffer
	{
	private:
		VkBuffer _indexBuffer;
		VkDeviceMemory _indexBufferMemory;
		VkAllocationCallbacks* _allocator;
		VkDevice _logicalDevice;
	public:
		IndexBuffer() = default;
		IndexBuffer(const VkDevice& logicalDevice, const uint16* indices, const uint32& bufferSize, VkAllocationCallbacks* allocator = nullptr);

		operator const VkBuffer () const & { return _indexBuffer; }

		void bind(const VkCommandBuffer& commandBuffer) const;
		void release() const;

		~IndexBuffer() = default;
	};
}