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
		void setClearColor(const glm::vec4& color) override;
		void setLineWidth(const float& width) override;
		void drawMesh(const components::mesh::Mesh& mesh) override;
		void drawMeshInstanced(const components::mesh::Mesh& mesh, renderer::InstanceData instanceData[], uint32 instanceCount = 1, uint32 startInstance = 0) override;
		void drawPrimitivesIndexed(const components::mesh::MeshTopology& type, uint32 indexBuffer[], uint32 indexCount, uint32 startIndex = 0, uint32 instanceCount = 1) override;

		~VulkanRendererAPI() = default;
	};
}