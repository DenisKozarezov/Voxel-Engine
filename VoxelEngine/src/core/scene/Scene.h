#pragma once
#include <core/renderer/Renderer.h>
#include "../Timestep.h"
#include <vulkan/vkUtils/VulkanMaterials.h>

namespace VoxelEngine
{
	struct MeshesCache
	{
		renderer::mesh::Mesh editorGrid;
		SharedRef<Mesh> mesh;
		Mesh voxel;
	};

	struct MaterialsCache
	{
		const renderer::mesh::IMaterial* solid;
		const renderer::mesh::IMaterial* wireframe;
		const renderer::mesh::IMaterial* normals;
	};

	class Scene
	{
	private:
		MeshesCache meshes;
		MaterialsCache materials;
		SharedRef<renderer::VertexBuffer> instancedBuffer;

		void prepareTestInstancedMesh();
		void prepareTestOctree();
	public:
		Scene();
		~Scene();
		Scene(Scene const&) noexcept = delete;
		Scene(Scene&&) noexcept = delete;
		Scene& operator=(Scene const& rhs) noexcept = delete;
		Scene& operator=(Scene&& rhs) noexcept = delete;

		void update(const Timestep& ts, components::camera::Camera& camera);
		void renderScene();
	};
}