#include "Scene.h"
#include <core/voxels/Octree.h>
#include <assets_management/AssetsProvider.h>
#include <VModel.h>
#include <components/mesh/MeshPrimitives.h>
#include <vulkan/vkUtils/VulkanMaterials.h>

#define TEST_INSTANCED_MESH 0

namespace VoxelEngine
{
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

		auto VModelMesh1 = VModel::Sphere(15.0f, { 30, 30, 25 }, { 50, 50, 50 });
		auto VModelMesh2 = VModel::Torus(15.0f, 10.0f, { 25, 25, 25 }, { 50, 50, 50 });
		auto VModelMesh3 = VModel::Plane({ 22, 28, 18 }, {22, 27, 22}, { 28, 26, 16 }, { 50, 50, 50 });
		auto result = VModel::Operations::Sum(VModelMesh1, VModelMesh3);
		auto points = VModelMesh3.GetPoints();
		instancesCount = static_cast<uint32>(points.size());

		std::vector<renderer::InstanceData> instanceData;
		instanceData.reserve(instancesCount);
		for (size_t i = 0; i < instancesCount; ++i)
		{
			instanceData.push_back(renderer::InstanceData{
				.pos = glm::vec3(points[i].x, points[i].y, points[i].z)
			});
		}
		instancedBuffer = renderer::VertexBuffer::Allocate(instanceData.data(), instancesCount * sizeof(renderer::InstanceData));
	}
#endif

	void Scene::release()
	{
		meshes.loadedModel.reset();
		delete meshes.svo;
	}
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

		input::EventDispatcher::registerEvent<input::MeshLoadedEvent>(BIND_CALLBACK(onMeshLoaded));

#if TEST_INSTANCED_MESH
		prepareTestInstancedMesh();
		materials.solid = utils::getMaterial("solid_instanced");
		materials.wireframe = utils::getMaterial("wireframe_instanced");
		materials.normals = utils::getMaterial("normals_instanced");
#endif
	}
	Scene::~Scene()
	{
		release();
	}

	bool Scene::onMeshLoaded(const input::MeshLoadedEvent& e)
	{
		release();

		auto& mesh = e.getLoadedMesh();
		meshes.loadedModel = mesh;
		meshes.svo = new Octree(mesh, 3);
		materials.solid = utils::getMaterial("solid");
		materials.wireframe = utils::getMaterial("wireframe");
		materials.normals = utils::getMaterial("normals");
		return true;
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

		// Show voxel grid
		utils::Gizmos::drawWireframeCube({ 25, 25, 25 }, { 50, 50, 50 });
#endif

		if (meshes.loadedModel)
		{
			switch (renderSettings.renderMode)
			{
			case renderer::Solid:
				meshes.loadedModel->material = materials.solid;
				break;
			case renderer::Wireframe:
				meshes.loadedModel->material = materials.wireframe;
				break;
			case renderer::Normals:
				meshes.loadedModel->material = materials.normals;
				break;
			}
			renderer::RenderCommand::drawMeshIndexed(*meshes.loadedModel.get());

			if (meshes.svo && renderSettings.showOctree)
			{
				meshes.svo->traverse([&](OctreeNode* node)
				{
					if (!node->isLeaf())
						return;

					glm::vec3 dimensions = node->bounds.max() - node->bounds.min();
					utils::Gizmos::drawWireframeCube(node->bounds.min() + dimensions * 0.5f, dimensions);
				});
			}
		}

		renderer::Renderer::render();
	}
}