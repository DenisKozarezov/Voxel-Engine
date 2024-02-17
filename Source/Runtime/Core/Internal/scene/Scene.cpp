#include "Scene.h"
#include <Core/Math/Octree.h>
#include <Renderer/RenderCommand.h>
#include <Vulkan/vkUtils/VulkanMaterials.h>
#include <Engine/Tests/VModelsTests.h>

#define TEST_INSTANCED_MESH 0

#ifdef TEST_INSTANCED_MESH
	#include <Renderer/GizmosAPI.h>
	#include <Renderer/RenderingStructs.h>
#endif

namespace VoxelEngine
{
#if TEST_INSTANCED_MESH
	uint32 instancesCount;

	void Scene::prepareTestInstancedMesh()
	{
		const glm::ivec3 areaSize = { 50, 50, 50 };
		auto VModelMesh1 = vmodel::Sphere(15.0, { 30, 30, 25 }, areaSize);
		auto VModelMesh2 = vmodel::Torus(15.0, 10.0, { 25, 25, 25 }, areaSize);
		auto VModelMesh3 = vmodel::Plane({ 22, 28, 18 }, { 22, 27, 22 }, { 28, 26, 16 }, areaSize);
		auto result = vmodel::Operations::sum(VModelMesh1, VModelMesh3, 1.0);
		auto points = result.get_points();

		instancesCount = static_cast<uint32>(points.size());

		std::vector<InstanceData> instanceData;
		instanceData.reserve(instancesCount);
		for (size_t i = 0; i < instancesCount; ++i)
		{
			instanceData.push_back(InstanceData{
				.pos = glm::vec3(points[i].x, points[i].y, points[i].z)
			});
		}
		instancedBuffer = renderer::VertexBuffer::Allocate(instanceData.data(), instancesCount * sizeof(InstanceData));
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
		materials.normalsLines = utils::getMaterial("normals_lines_instanced");
#else
		materials.solid = utils::getMaterial("solid");
		materials.wireframe = utils::getMaterial("wireframe");
		materials.normals = utils::getMaterial("normals");
		materials.normalsLines = utils::getMaterial("normals_lines");
#endif
		
		auto mesh = CreateTestHomotopy({200, 200, 200});
		onMeshLoaded(mesh);
		registerMesh(mesh);
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
		const auto& renderSettings = renderer::Renderer::getRenderSettings();

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

		if (renderSettings.showNormalsLines)
		{
			meshes.voxel.material = materials.normalsLines;
			renderer::RenderCommand::drawMeshInstanced(meshes.voxel, instancedBuffer, instancesCount);
		}
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
					renderer::RenderCommand::drawMeshIndexed(*mesh);

				if (renderSettings.showNormalsLines)
				{
					mesh->material = materials.normalsLines;
					renderer::RenderCommand::drawMeshIndexed(*mesh);
				}
			}

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

			utils::Gizmos::drawWireframeCube({100, 100, 100}, {200, 200, 200});
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