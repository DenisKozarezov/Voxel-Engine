#include "Scene.h"
#include <Core/Math/Octree.h>
#include <Renderer/RenderCommand.h>
#include <Vulkan/vkUtils/VulkanMaterials.h>
#include <VModel.h>

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
		auto VModelMesh3 = VModel::Plane({ 22, 28, 18 }, { 22, 27, 22 }, { 28, 26, 16 }, { 50, 50, 50 });
		auto result = VModel::Operations::Sum(VModelMesh1, VModelMesh3);
		auto points = result.GetPoints();
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
		std::for_each(objects.begin(), objects.end(), [&](TSharedPtr<Mesh>& mesh) {
			unregisterMesh(mesh);
		});
		delete meshes.svo;
	}
	Scene::Scene()
	{
		meshes.editorGrid.material = utils::getMaterial("editor_grid");

#if TEST_INSTANCED_MESH
		prepareTestInstancedMesh();
		materials.solid = utils::getMaterial("solid_instanced");
		materials.wireframe = utils::getMaterial("wireframe_instanced");
		materials.normals = utils::getMaterial("normals_instanced");
		materials.normals = utils::getMaterial("normals_lines_instanced");
#else
		materials.solid = utils::getMaterial("solid");
		materials.wireframe = utils::getMaterial("wireframe");
		materials.normals = utils::getMaterial("normals");
		materials.normalsLines = utils::getMaterial("normals_lines");
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
		meshes.svo = new Octree(mesh, 3);
		return true;
	}

	void Scene::update(const Timestep& ts)
	{
		renderScene();
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

		if (!objects.empty())
		{
			for (const auto& mesh : objects)
			{
				switch (renderSettings.renderMode)
				{
				case renderer::Solid:
					mesh->material = materials.solid;
					break;
				case renderer::Wireframe:
					mesh->material = materials.wireframe;
					break;
				case renderer::Normals:
						mesh->material = materials.normals;
					break;
				}

				if (!mesh->material->instanced())
					renderer::RenderCommand::drawMeshIndexed(*mesh.get());

				if (renderSettings.showNormalsLines)
				{
					mesh->material = materials.normalsLines;
					renderer::RenderCommand::drawMeshIndexed(*mesh.get());
				}
			}

			/*if (meshes.svo && renderSettings.showOctree)
			{
				meshes.svo->traverse([&](OctreeNode* node)
				{
					if (!node->isLeaf())
						return;

					glm::vec3 dimensions = node->bounds.max() - node->bounds.min();
					utils::Gizmos::drawWireframeCube(node->bounds.min() + dimensions * 0.5f, dimensions);
				});
			}*/
		}
	}
	void Scene::registerMesh(const TSharedPtr<Mesh>& mesh)
	{
		const auto it = std::find(objects.begin(), objects.end(), mesh);
		if (it != objects.end())
		{
			RUNTIME_ERROR("Unable to register a new mesh! There is a duplicate.");
		}
		else
		{
			objects.emplace_back(mesh);
		}
	}
	void Scene::unregisterMesh(const TSharedPtr<Mesh>& mesh)
	{
		const auto it = std::find(objects.begin(), objects.end(), mesh);
		if (it != objects.end())
		{
			objects.erase(it);
		}
	}
}