#include "VertexManager.h"

namespace VoxelEngine::renderer
{
	VertexManager::VertexManager()
	{
		indexOffset = 0;
	}
	void VertexManager::concatMesh(
		const MeshTopology& type,
		const Mesh& mesh)
	{
		concatMesh(type, mesh.vertices, mesh.vertexCount, mesh.indices, mesh.indexCount);
	}
	void VertexManager::concatMesh(
		const MeshTopology& type, 
		const Vertex* vertices,
		uint32 vertexCount,
		const uint32* indices,
		uint32 indexCount)
	{
		size_t lastIndex = this->indices.size();

		firstIndices.insert(std::make_pair(type, lastIndex));
		indexCounts.insert(std::make_pair(type, indexCount));

		for (uint32 i = 0; i < vertexCount; ++i)
			this->vertices.push_back(vertices[i]);

		for (uint32 i = 0; i < indexCount; ++i)
			this->indices.push_back(indices[i] + indexOffset);

		indexOffset += vertexCount;
	}
	void VertexManager::finalize(const VkPhysicalDevice& physicalDevice, const VkDevice& logicalDevice)
	{
		VkDeviceSize verticesSize = sizeof(Vertex) * vertices.size();
		vertexBuffer = new vkUtils::VulkanVertexBuffer(physicalDevice, logicalDevice, vertices.data(), verticesSize);
	
		VkDeviceSize indicesSize = sizeof(uint32) * indices.size();
		indexBuffer = new vkUtils::VulkanIndexBuffer(physicalDevice, logicalDevice, indices.data(), indicesSize);
	}
	VertexManager::~VertexManager()
	{
		delete vertexBuffer;
		delete indexBuffer;
	}
}