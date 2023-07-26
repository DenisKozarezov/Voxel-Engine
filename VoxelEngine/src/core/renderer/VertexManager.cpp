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
		VkDeviceSize size = sizeof(vertices[0]) * vertices.size();
		vertexBuffer = new vkUtils::VulkanVertexBuffer(physicalDevice, logicalDevice, vertices.data(), size);
	}
	VertexManager::~VertexManager()
	{
		delete vertexBuffer;
	}
}