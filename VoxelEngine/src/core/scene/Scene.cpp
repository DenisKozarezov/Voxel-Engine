#include "Scene.h"
#include <core/voxels/Octree.h>
#include <assets_management/AssetsProvider.h>
#include <VModels.h>
#include <components/mesh/MeshPrimitives.h>
#include <vulkan/VulkanBackend.h>

#define TEST_INSTANCED_MESH 0
#define TEST_OCTREE 0
#define TEST_RAYMARCHING 1

namespace VoxelEngine
{
#if TEST_OCTREE
	Octree* svo = nullptr;

	void Scene::prepareTestOctree()
	{
		meshes.mesh = assets::AssetsProvider::loadObjMesh(ASSET_PATH("models/FinalBaseMesh.obj"));
		svo = new Octree(meshes.mesh, 3);
	}
#endif

#if TEST_INSTANCED_MESH
	uint32 instancesCount;

	void Scene::prepareTestInstancedMesh()
	{
		meshes.voxel = renderer::mesh::VoxelMesh();

		auto* vertices = meshes.voxel.vertices.data();
		auto* indices = meshes.voxel.indices.data();
		uint32 vertexCount = meshes.voxel.vertexCount();
		uint32 indexCount = meshes.voxel.indexCount();

		meshes.voxel.vertexBuffer = renderer::VertexBuffer::Allocate(vertices, vertexCount * sizeof(renderer::Vertex));
		meshes.voxel.indexBuffer = renderer::IndexBuffer::Allocate(indices, indexCount * sizeof(uint32));

		const auto& VModelMesh = VModels::Primitives::Sphere(10);
		instancesCount = static_cast<uint32>(VModelMesh.size());

		std::vector<renderer::InstanceData> instanceData;
		instanceData.reserve(instancesCount);
		for (size_t i = 0; i < instancesCount; ++i)
		{
			instanceData.push_back(renderer::InstanceData{
				.pos = glm::vec3(VModelMesh[i].x, VModelMesh[i].y, VModelMesh[i].z)
			});
		}
		instancedBuffer = renderer::VertexBuffer::Allocate(instanceData.data(), instancesCount * sizeof(renderer::InstanceData));
	}
#endif

	Scene::Scene()
	{
		meshes.editorGrid = renderer::mesh::QuadMesh();

		auto* vertices = meshes.editorGrid.vertices.data();
		auto* indices = meshes.editorGrid.indices.data();
		uint32 vertexCount = meshes.editorGrid.vertexCount();
		uint32 indexCount = meshes.editorGrid.indexCount();

		meshes.editorGrid.vertexBuffer = VoxelEngine::renderer::VertexBuffer::Allocate(vertices, vertexCount * sizeof(renderer::Vertex));
		meshes.editorGrid.indexBuffer = VoxelEngine::renderer::IndexBuffer::Allocate(indices, indexCount * sizeof(uint32));
		meshes.editorGrid.material = utils::getMaterial("editor_grid");	

#if TEST_INSTANCED_MESH
		prepareTestInstancedMesh();
		materials.solid = utils::getMaterial("solid_instanced");
		materials.wireframe = utils::getMaterial("wireframe_instanced");
		materials.normals = utils::getMaterial("normals_instanced");
#endif

#if TEST_OCTREE
		prepareTestOctree();
		materials.solid = utils::getMaterial("solid");
		materials.wireframe = utils::getMaterial("wireframe");
		materials.normals = utils::getMaterial("normals");
#endif

#if TEST_RAYMARCHING
		materials.raymarchQuad = utils::getMaterial("raymarch_quad");
#endif
	}
	Scene::~Scene()
	{
#if TEST_OCTREE
		delete svo;
#endif
	}

	void Scene::update(const Timestep& ts, components::camera::Camera& camera)
	{
		renderer::Renderer::resetStats();

		renderer::Renderer::preRender(camera);

		renderScene();

		renderer::Renderer::postRender();
	}
	void Scene::renderScene()
	{
#if TEST_RAYMARCHING
		renderer::RenderCommand::draw(materials.raymarchQuad, 3);
#endif
		auto& renderSettings = renderer::Renderer::getRenderSettings();
		
		if (renderSettings.showEditorGrid)
		{
			renderer::RenderCommand::drawMeshIndexed(meshes.editorGrid);
		}

#if TEST_INSTANCED_MESH
		switch (renderSettings.renderMode)
		{
		case renderer::Solid:
			meshes.voxel.material = materials.solid;
			break;
		case renderer::Wireframe:
			meshes.voxel.material = materials.wireframe;
			break;
		case renderer::Normals:
			meshes.voxel.material = materials.normals;
			break;
		}
		renderer::RenderCommand::drawMeshInstanced(meshes.voxel, instancedBuffer, instancesCount);
#endif

#if TEST_OCTREE
		switch (renderSettings.renderMode)
		{
		case renderer::Solid:
			meshes.mesh->material = materials.solid;
			break;
		case renderer::Wireframe:
			meshes.mesh->material = materials.wireframe;
			break;
		case renderer::Normals:
			meshes.mesh->material = materials.normals;
			break;
		}
		renderer::RenderCommand::drawMeshIndexed(*meshes.mesh.get());
				
		svo->traverse([&](OctreeNode* node)
		{
			if (!node->isLeaf())
				return;

			glm::vec3 dimensions = node->bounds.max() - node->bounds.min();
			utils::Gizmos::drawWireframeCube(node->bounds.min() + dimensions * 0.5f, dimensions);
		});
#endif

		renderer::Renderer::render();
	}
}