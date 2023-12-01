#pragma once
#include <core/renderer/RendererAPI.h>

namespace vulkan
{
	using namespace VoxelEngine;

	class VulkanRendererAPI : public renderer::RendererAPI
	{
	public:
		VulkanRendererAPI() noexcept = default;

		void init(const Window& window) override;
		void setViewport(const uint32& x, const uint32& y, const uint32& width, const uint32& height) override;
		void setClearColor(const glm::vec4 color) override;
		void setLineWidth(const float& width) override;
		void draw(uint32 vertexCount, uint32 instanceCount = 1, uint32 startVertex = 0, uint32 startInstance = 0) override;
		INLINE void drawMesh(const components::mesh::Mesh& mesh) override
		{
			draw(mesh.vertexCount(), 1, 0, 0);
		}
		INLINE void drawMeshIndexed(const components::mesh::Mesh& mesh, uint32 instanceCount = 1, uint32 startIndex = 0, uint32 startInstance = 0) override
		{
			drawMeshIndexed(mesh.indexCount(), instanceCount, startIndex, startInstance);
		}
		void drawMeshIndexed(uint32 indexCount, uint32 instanceCount = 1, uint32 startIndex = 0, uint32 startInstance = 0) override;

		~VulkanRendererAPI() = default;
	};
}