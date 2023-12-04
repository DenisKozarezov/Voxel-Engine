#pragma once
#include <core/renderer/Renderer.h>
#include <core/input/events/ApplicationEvent.h>
#include <core/input/events/EventDispatcher.h>
#include "../Timestep.h"

namespace VoxelEngine
{
	class Octree;
	class renderer::mesh::IMaterial;
	struct components::mesh::Mesh;

	struct MeshesCache
	{
		components::mesh::Mesh editorGrid;
		components::mesh::Mesh voxel;
		Octree* svo = nullptr;
		SharedRef<components::mesh::Mesh> loadedModel;
	};

	struct MaterialsCache
	{
		const renderer::mesh::IMaterial* solid;
		const renderer::mesh::IMaterial* wireframe;
		const renderer::mesh::IMaterial* normals;
		const renderer::mesh::IMaterial* raymarchQuad;
	};

	class Scene
	{
	private:
		MeshesCache meshes;
		MaterialsCache materials;
		SharedRef<renderer::VertexBuffer> instancedBuffer;

		void prepareTestInstancedMesh();
		void release();
	public:
		Scene();
		~Scene();
		Scene(Scene const&) noexcept = delete;
		Scene(Scene&&) noexcept = delete;
		Scene& operator=(Scene const& rhs) noexcept = delete;
		Scene& operator=(Scene&& rhs) noexcept = delete;

		bool onMeshLoaded(const input::MeshLoadedEvent& e);
		void update(const Timestep& ts, components::camera::Camera& camera);
		void renderScene();
	};
}