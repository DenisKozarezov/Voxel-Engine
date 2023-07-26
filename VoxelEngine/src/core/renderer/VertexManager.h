#pragma once
#include <vector>
#include <unordered_map>
#include <vulkan/vkUtils/VulkanVertexBuffer.h>
#include <components/mesh/Mesh.h>

namespace VoxelEngine::renderer
{
	using namespace components::mesh;

	class VertexManager
	{
	private:
		VkDevice logicalDevice;
		size_t offset = 0;
	public:
		vkUtils::VulkanVertexBuffer* vertexBuffer;
		std::vector<vulkan::Vertex> vertices;
		std::unordered_map<MeshType, size_t> offsets;
		std::unordered_map<MeshType, size_t> sizes;

		VertexManager();
		VertexManager(VertexManager const&) noexcept = delete;
		VertexManager(VertexManager&&) noexcept = delete;
		VertexManager& operator=(VertexManager const& rhs) noexcept = delete;
		VertexManager& operator=(VertexManager&& rhs) noexcept = delete;

		void concatMesh(const MeshType& type, const std::vector<vulkan::Vertex>& vertices);
		void finalize(const VkPhysicalDevice& physicalDevice, const VkDevice& logicalDevice);

		~VertexManager();
	};
}