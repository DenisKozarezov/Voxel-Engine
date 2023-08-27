#pragma once
#include "RendererAPI.h"

namespace VoxelEngine::renderer
{
	class RenderCommand
	{
	private:
		static UniqueRef<RendererAPI> s_renderer;
	public:
		static void init(const Window& window);
		static void setViewport(const int32_t& x, const int32_t& y, const uint32& width, const uint32& height);
		static void setClearColor(const glm::vec4& color);
		static void drawMesh(const mesh::Mesh& mesh);
		static void drawMeshInstanced(const mesh::Mesh& mesh, InstanceData instanceData[], uint32 instanceCount = 1, uint32 startInstance = 0);
		static void drawPrimitivesIndexed(const mesh::MeshTopology& type, uint32 indexBuffer[], uint32 indexCount, uint32 startIndex = 0, uint32 instanceCount = 1);
	};
}