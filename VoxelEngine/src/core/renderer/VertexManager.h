#pragma once
#include <pch.h>
#include <vulkan/vkUtils/VulkanVertexBuffer.h>
#include <vulkan/vkUtils/VulkanIndexBuffer.h>
#include <components/mesh/Mesh.h>

namespace VoxelEngine::renderer
{
	using namespace components::mesh;

	class VertexManager
	{
	private:
		size_t indexOffset = 0;
		std::vector<uint32> indices;		
	public:
		std::vector<vulkan::Vertex> vertices;
		vkUtils::VulkanVertexBuffer* vertexBuffer = nullptr;
		vkUtils::VulkanIndexBuffer* indexBuffer = nullptr;
		std::unordered_map<MeshType, size_t> firstIndices;
		std::unordered_map<MeshType, size_t> indexCounts;

		VertexManager();
		VertexManager(VertexManager const&) noexcept = delete;
		VertexManager(VertexManager&&) noexcept = delete;
		VertexManager& operator=(VertexManager const& rhs) noexcept = delete;
		VertexManager& operator=(VertexManager&& rhs) noexcept = delete;

		inline constexpr bool isValid() const { return vertexBuffer && indexBuffer; };

		void concatMesh(
			const MeshType& type, 
			const std::vector<vulkan::Vertex>& vertices, 
			const std::vector<uint32>& indices);
		void finalize(const VkPhysicalDevice& physicalDevice, const VkDevice& logicalDevice);

		~VertexManager();
	};
}