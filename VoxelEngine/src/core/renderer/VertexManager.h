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
		static constexpr uint32 MAX_TRIANGLES = 20000;
		static constexpr uint32 MAX_VERTICES = MAX_TRIANGLES * 3;
		static constexpr uint32 MAX_INDICES = MAX_TRIANGLES * 3;

		size_t indexOffset = 0;
		std::vector<uint32> indices;		
	public:
		std::vector<Vertex> vertices;
		vkUtils::VulkanVertexBuffer* vertexBuffer = nullptr;
		vkUtils::VulkanIndexBuffer* indexBuffer = nullptr;
		std::unordered_map<MeshTopology, size_t> firstIndices;
		std::unordered_map<MeshTopology, size_t> indexCounts;

		VertexManager();
		VertexManager(VertexManager const&) noexcept = delete;
		VertexManager(VertexManager&&) noexcept = delete;
		VertexManager& operator=(VertexManager const& rhs) noexcept = delete;
		VertexManager& operator=(VertexManager&& rhs) noexcept = delete;

		inline constexpr bool isValid() const { return vertexBuffer && indexBuffer; };

		void concatMesh(
			const MeshTopology& type,
			const std::vector<Vertex>& vertices, 
			const std::vector<uint32>& indices);
		void finalize(const VkPhysicalDevice& physicalDevice, const VkDevice& logicalDevice);

		~VertexManager();
	};
}