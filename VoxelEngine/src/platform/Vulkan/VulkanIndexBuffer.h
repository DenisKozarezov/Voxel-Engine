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
		IndexBuffer(const uint16* indices, const uint32& size, VkAllocationCallbacks* allocator = nullptr);

		void bind() const;
		void release() const;

		~IndexBuffer() = default;
	};
}