#include <vector>
#include <vulkan/VulkanVertex.h>
#include <vulkan/vkUtils/VulkanAlloc.h>

namespace VoxelEngine::components::mesh
{
	struct SquareMesh
	{
	private:
		VkDevice _logicalDevice;
	public:
		vkUtils::memory::Buffer vertexBuffer;

		SquareMesh(const VkDevice& logicalDevice, const VkPhysicalDevice& physicalDevice)
			: _logicalDevice(logicalDevice)
		{
			const std::vector<vulkan::Vertex> vertices = {
				{{-0.05f,  0.05f, 1.0f}, {0.0f, 0.0f, 0.0f}},
				{{-0.05f, -0.05f, 1.0f}, {0.0f, 0.0f, 0.0f}},
				{{0.05f, -0.05f, 1.0f}, {0.0f, 0.0f,0.0f}},
				{{0.05f, -0.05f, 1.0f}, {0.0f, 0.0f,0.0f}},
				{{0.05f,  0.05f, 1.0f}, {0.0f, 0.0f, 0.0f}},
				{{-0.05f,  0.05f, 1.0f}, {0.0f, 0.0f, 0.0f}}
			};

			VkDeviceSize size = sizeof(vertices[0]) * vertices.size();

			vertexBuffer = vkUtils::memory::createBuffer(
				physicalDevice,
				logicalDevice,
				size,
				VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

			vkUtils::memory::mapMemory(
				logicalDevice,
				vertexBuffer.bufferMemory,
				0,
				size,
				0,
				vertices.data());
		}

		~SquareMesh()
		{
			vkUtils::memory::destroyBuffer(_logicalDevice, vertexBuffer.buffer);
			vkUtils::memory::freeDeviceMemory(_logicalDevice, vertexBuffer.bufferMemory);
		}
	};
}