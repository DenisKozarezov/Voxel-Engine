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
		alignas(16) glm::mat4 proj;
	};

	class UniformBuffer
	{
	public:
		VkBuffer _uniformBuffer;
		VkDeviceMemory _uniformBufferMemory;
		void* _uniformBufferMapped;
		VkDevice _logicalDevice;
		uint32 _bufferSize;
	public:
		UniformBuffer() = default;
		UniformBuffer(const VkDevice& logicalDevice, const uint32& size, VkAllocationCallbacks* allocator = nullptr);

		operator VkBuffer() const & { return _uniformBuffer; }

		void setData(const void* data, size_t size) const;
		void release() const;

		~UniformBuffer() = default;
	};
}