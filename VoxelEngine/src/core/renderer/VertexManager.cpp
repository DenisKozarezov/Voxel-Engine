#include "VertexManager.h"

namespace VoxelEngine::renderer
{
	VertexManager::VertexManager()
	{
		indexOffset = 0;
	}
	void VertexManager::concatMesh(
		const MeshType& type, 
		const std::vector<vulkan::Vertex>& vertices, 
		const std::vector<uint32>& indices)
	{
		size_t vertexCount = vertices.size();
		size_t indexCount = indices.size();
		size_t lastIndex = this->indices.size();

		firstIndices.insert(std::make_pair(type, lastIndex));
		indexCounts.insert(std::make_pair(type, indexCount));

		for (const auto& vertex : vertices) 
		{
			this->vertices.push_back(vertex);
		}

		for (const uint32& index : indices)
		{
			this->indices.push_back(index + indexOffset);
		}

		indexOffset += vertexCount;
	}
	void VertexManager::finalize(const VkPhysicalDevice& physicalDevice, const VkDevice& logicalDevice)
	{
		VkDeviceSize verticesSize = sizeof(vertices[0]) * vertices.size();
		vertexBuffer = new vkUtils::VulkanVertexBuffer(physicalDevice, logicalDevice, vertices.data(), verticesSize);
	
		VkDeviceSize indicesSize = sizeof(indices[0]) * indices.size();
		indexBuffer = new vkUtils::VulkanIndexBuffer(physicalDevice, logicalDevice, indices.data(), indicesSize);
	}
	VertexManager::~VertexManager()
	{
		delete vertexBuffer;
		delete indexBuffer;
	}
}