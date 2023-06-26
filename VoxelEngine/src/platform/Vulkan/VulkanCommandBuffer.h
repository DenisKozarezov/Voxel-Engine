#pragma once
#include <vector>
#include <vulkan/vulkan.h>
#include <core/PrimitiveTypes.h>

namespace VoxelEngine::renderer
{
	static class CommandBuffer
	{
	public:
		static VkCommandBuffer allocate();
		static std::vector<VkCommandBuffer> allocate(const uint32& buffersCount);

		static void reset(const VkCommandBuffer& buffer);
		static void beginCommand(const VkCommandBuffer& buffer);
		static void endCommand(const VkCommandBuffer& buffer);
		static void free(const VkCommandBuffer& buffer);
		static void free(const std::vector<VkCommandBuffer>& buffers);
	};
}