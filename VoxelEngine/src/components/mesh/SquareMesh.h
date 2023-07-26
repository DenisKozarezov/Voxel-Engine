#include <vector>
#include <vulkan/VulkanVertex.h>
#include <vulkan/VulkanAlloc.h>

namespace VoxelEngine::components::mesh
{
	struct SquareMesh
	{
	private:
		VkDevice _logicalDevice;
	public:
		vulkan::memory::Buffer vertexBuffer;

		SquareMesh(const VkDevice& logicalDevice, const VkPhysicalDevice& physicalDevice)
			: _logicalDevice(logicalDevice)
		{
			const std::vector<vulkan::Vertex> vertices = {
				{{-0.05f,  0.05f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
				{{-0.05f, -0.05f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
				{{0.05f, -0.05f, 1.0f}, {0.0f, 0.0f,0.0f}, {0.0f, 0.0f}},
				{{0.05f, -0.05f, 1.0f}, {0.0f, 0.0f,0.0f}, {0.0f, 0.0f}},
				{{0.05f,  0.05f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
				{{-0.05f,  0.05f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}}
			};

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

		~SquareMesh()
		{
			vulkan::memory::destroyBuffer(_logicalDevice, vertexBuffer.buffer);
			vulkan::memory::freeDeviceMemory(_logicalDevice, vertexBuffer.bufferMemory);
		}
	};
}