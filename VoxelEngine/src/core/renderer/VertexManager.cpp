#include "VertexManager.h"

namespace VoxelEngine::renderer
{
	VertexManager::VertexManager()
	{
		offset = 0;
	}
	void VertexManager::concatMesh(const MeshType& type, const std::vector<vulkan::Vertex>& vertices)
	{
		for (const auto& vertex : vertices) 
		{
			this->vertices.push_back(vertex);
		}

		size_t vertexCount = vertices.size();

		offsets.insert(std::make_pair(type, offset));
		sizes.insert(std::make_pair(type, vertexCount));

		offset += vertexCount;
	}
	void VertexManager::finalize(const VkPhysicalDevice& physicalDevice, const VkDevice& logicalDevice)
	{
		this->logicalDevice = logicalDevice;

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
	VertexManager::~VertexManager()
	{
		vulkan::memory::destroyBuffer(logicalDevice, vertexBuffer.buffer);
		vulkan::memory::freeDeviceMemory(logicalDevice, vertexBuffer.bufferMemory);
	}
}