#pragma once
#include <Core/Internal/Window.h>
#include <Engine/Components/mesh/Mesh.h>

namespace VoxelEngine::renderer
{
	namespace mesh = VoxelEngine::components::mesh;

	enum class GraphicsSpec : byte
	{
		None = 0,
		OpenGL = 1,
		Vulkan = 2,
		DirectX12 = 3,
	};

	struct InstanceData
	{
		glm::vec3 pos;
	};

	class RendererAPI
	{
	public:
		RendererAPI() noexcept = default;
		virtual ~RendererAPI() = default;

		virtual void init(const VoxelEngine::Window& window) = 0;
		virtual void setViewport(const uint32& x, const uint32& y, const uint32& width, const uint32& height) = 0;
		virtual void setClearColor(const glm::vec4 color) = 0;
		virtual void setLineWidth(const float& width) = 0;
		virtual void draw(uint32 vertexCount, uint32 instanceCount = 1, uint32 startVertex = 0, uint32 startInstance = 0) = 0;
		virtual void drawMesh(const mesh::Mesh& mesh) = 0;
		virtual void drawMeshIndexed(const mesh::Mesh& mesh, uint32 instanceCount = 1, uint32 startIndex = 0, uint32 startInstance = 0) = 0;
		virtual void drawMeshIndexed(uint32 indexCount, uint32 instanceCount = 1, uint32 startIndex = 0, uint32 startInstance = 0) = 0;

		static UniqueRef<RendererAPI> Create();
	};

	extern const GraphicsSpec g_graphicsSpec;
}