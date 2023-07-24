#pragma once
#include <list>
#include <vector>
#include <vulkan/VulkanVertex.h>
#include <vulkan/VulkanAlloc.h>

namespace VoxelEngine::components::mesh
{
	struct Mesh
	{
		std::list<vulkan::Vertex> vertices;
		std::list<uint32> indices;

		Mesh() noexcept = default;

		~Mesh()
		{

		}
	};

	struct TriangleMesh
	{
	private:
		VkDevice _logicalDevice;

		const std::vector<vulkan::Vertex> vertices = {
			{{0.0f, -0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
			{{0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
			{{-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}
		};
	public:
		vulkan::memory::Buffer vertexBuffer;

		TriangleMesh(const VkDevice& logicalDevice, const VkPhysicalDevice& physicalDevice)
			: _logicalDevice(logicalDevice)
		{
			VkDeviceSize size = sizeof(vertices[0]) * vertices.size();

			vertexBuffer = vulkan::memory::createBuffer(
				physicalDevice,
				logicalDevice,
				size,
				VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

			vulkan::memory::mapMemory(
				logicalDevice, 
				vertexBuffer.bufferMemory, 
				0, 
				size, 
				0, 
				vertices.data());
		}

		~TriangleMesh()
		{
			vulkan::memory::destroyBuffer(_logicalDevice, vertexBuffer.buffer);
			vulkan::memory::freeDeviceMemory(_logicalDevice, vertexBuffer.bufferMemory);
		}
	};
}