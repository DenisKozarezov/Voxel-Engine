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
		void drawMesh(const components::mesh::Mesh& mesh) override;
		void drawMeshIndexed(const components::mesh::Mesh& mesh, uint32 instanceCount = 1, uint32 startIndex = 0, uint32 startInstance = 0) override;
		void drawMeshIndexed(uint32 indexCount, uint32 instanceCount = 1, uint32 startIndex = 0, uint32 startInstance = 0) override;

		~VulkanRendererAPI() = default;
	};
}