#pragma once
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/mat4x4.hpp>
#include <vulkan/vulkan.h>
#include <core/PrimitiveTypes.h>

namespace VoxelEngine::renderer
{
	struct UniformBufferObject
	{
		alignas(16) glm::mat4 model;
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 projection;
	};

	class UniformBuffer
	{
	private:
		VkBuffer _uniformBuffer;
		VkDeviceMemory _uniformBufferMemory;
		void* _uniformBufferMapped;
		VkDevice _logicalDevice;
		uint32 _bufferSize;
		VkAllocationCallbacks* _allocator;
	public:
		UniformBuffer() = default;
		UniformBuffer(const uint32& size, VkAllocationCallbacks* allocator = nullptr);

		operator VkBuffer() const & { return _uniformBuffer; }

		void setData(void* data) const;
		void release() const;

		~UniformBuffer() = default;
	};
}