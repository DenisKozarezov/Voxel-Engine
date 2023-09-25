#include "Scene.h"
#include <core/voxels/Octree.h>
#include <assets_management/AssetsProvider.h>
#include <vulkan/vkUtils/VulkanGizmos.h>
#include <vulkan/vkUtils/VulkanMaterials.h>
#include <VModels.h>

namespace VoxelEngine
{
	Octree* svo = nullptr;
	renderer::mesh::Mesh editorGrid;
	SharedRef<Mesh> mesh;
	Mesh voxel;
	SharedRef<renderer::VertexBuffer> instancedBuffer;
	uint32 instancesCount;

	struct MaterialsCache
	{
		const renderer::mesh::IMaterial* solid;
		const renderer::mesh::IMaterial* wireframe;
		const renderer::mesh::IMaterial* normals;
	} materials;

	void Scene::prepareTestInstancedMesh()
	{
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
		instancedBuffer = renderer::VertexBuffer::Allocate(instanceData.data(), sizeof(renderer::InstanceData) * instancesCount);
	}
	void Scene::prepareTestOctree()
	{
		mesh = assets::AssetsProvider::loadObjMesh(ASSET_PATH("models/FinalBaseMesh.obj"));
		svo = new Octree(mesh, 3);
	}

	Scene::Scene()
	{
		editorGrid = renderer::mesh::QuadMesh();
		editorGrid.vertexBuffer = VoxelEngine::renderer::VertexBuffer::Allocate(editorGrid.vertices, editorGrid.vertexCount * sizeof(renderer::Vertex));
		editorGrid.indexBuffer = VoxelEngine::renderer::IndexBuffer::Allocate(editorGrid.indices, editorGrid.indexCount * sizeof(uint32));
		editorGrid.material = utils::getMaterial("editor_grid");

		voxel = renderer::mesh::VoxelMesh();
		voxel.vertexBuffer = renderer::VertexBuffer::Allocate(voxel.vertices, voxel.vertexCount * sizeof(renderer::Vertex));
		voxel.indexBuffer = renderer::IndexBuffer::Allocate(voxel.indices, voxel.indexCount * sizeof(uint32));

		materials.solid = utils::getMaterial("solid_instanced");
		materials.wireframe = utils::getMaterial("wireframe_instanced");
		materials.normals = utils::getMaterial("normals");

		prepareTestInstancedMesh();
	}
	Scene::~Scene()
	{
		delete svo;
		editorGrid.release();
		//mesh->release();
		voxel.release();
		instancedBuffer->release();
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
			renderer::RenderCommand::drawMeshIndexed(editorGrid);
		}
		
		switch (renderSettings.renderMode)
		{
		case renderer::Solid:
			voxel.material = materials.solid;
			break;
		case renderer::Wireframe:
			voxel.material = materials.wireframe;
			break;
		case renderer::Normals:
			voxel.material = materials.normals;
			break;
		}
		renderer::RenderCommand::drawMeshInstanced(voxel, instancedBuffer, instancesCount);

		//renderer::RenderCommand::drawMeshIndexed(*mesh.get());
				
		//svo->traverse([&](OctreeNode* node)
		//{
		//	if (!node->isLeaf())
		//		return;

		//	glm::vec3 dimensions = node->box.max() - node->box.min();
		//	utils::Gizmos::drawWireframeCube(node->box.min() + dimensions * 0.5f, dimensions);
		//});
	}
}