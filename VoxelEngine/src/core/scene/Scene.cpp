#include "Scene.h"
#include <core/voxels/Octree.h>
#include <assets_management/AssetsProvider.h>
#include <vulkan/vkUtils/VulkanGizmos.h>
#include <vulkan/vkUtils/VulkanMaterials.h>

namespace VoxelEngine
{
	Octree* svo;
	renderer::mesh::Mesh editorGrid;
	SharedRef<Mesh> mesh;

	struct MaterialsCache
	{
		const renderer::mesh::IMaterial* solid;
		const renderer::mesh::IMaterial* wireframe;
		const renderer::mesh::IMaterial* normals;
	} materials;

	Scene::Scene()
	{
		editorGrid = renderer::mesh::QuadMesh();
		editorGrid.vertexBuffer = VoxelEngine::renderer::VertexBuffer::Allocate(editorGrid.vertices, editorGrid.vertexCount * sizeof(renderer::Vertex));
		editorGrid.indexBuffer = VoxelEngine::renderer::IndexBuffer::Allocate(editorGrid.indices, editorGrid.indexCount * sizeof(uint32));
		editorGrid.material = utils::getMaterial("editor_grid");

		materials.solid = utils::getMaterial("solid_instanced");
		materials.wireframe = utils::getMaterial("wireframe_instanced");
		materials.normals = utils::getMaterial("normals");

		mesh = assets::AssetsProvider::loadObjMesh(ASSET_PATH("models/FinalBaseMesh.obj"));
		svo = new Octree(mesh, 3);
	}
	Scene::~Scene()
	{
		delete svo;
		editorGrid.release();
		mesh->release();
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
			editorGrid.material->bind();
			renderer::RenderCommand::drawMeshIndexed(editorGrid);
		}
		
		switch (renderSettings.renderMode)
		{
		case renderer::Solid:
			materials.solid->bind();
			break;
		case renderer::Wireframe:
			materials.wireframe->bind();
			break;
		case renderer::Normals:
			materials.normals->bind();
			break;
		}
		renderer::RenderCommand::drawMeshIndexed(*mesh.get());
				
		svo->traverse([&](OctreeNode* node)
		{
			if (!node->isLeaf())
				return;

			glm::vec3 dimensions = node->box.max() - node->box.min();
			utils::Gizmos::drawWireframeCube(node->box.min() + dimensions * 0.5f, dimensions);
		});
	}
}