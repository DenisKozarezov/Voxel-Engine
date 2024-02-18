#pragma once
#include <InputCore/events/ApplicationEvent.h>
#include <InputCore/events/EventDispatcher.h>
#include <Engine/Components/mesh/MeshPrimitives.h>
#include "../Timestep.h"

namespace VoxelEngine
{
	class Octree;
	class renderer::IMaterial;
	using Mesh = components::mesh::Mesh;

	struct MeshesCache
	{
		components::mesh::QuadMesh editorGrid;
		components::mesh::VoxelMesh voxel;
		Octree* svo = nullptr;
	};

	struct MaterialsCache
	{
		TSharedPtr<const renderer::IMaterial> solid;
		TSharedPtr<const renderer::IMaterial> wireframe;
		TSharedPtr<const renderer::IMaterial> normals;
		TSharedPtr<const renderer::IMaterial> normalsLines;
	};

	class Scene
	{
	private:
		MeshesCache meshes;
		MaterialsCache materials;
		TSharedPtr<renderer::VertexBuffer> instancedBuffer;
		std::vector<TSharedPtr<Mesh>> objects;

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
		void update(const Timestep& ts);
		void renderScene();
		
		void registerMesh(const TSharedPtr<Mesh>& mesh);
		void unregisterMesh(const TSharedPtr<Mesh>& mesh);
	};
}