#include "Scene.h"
#include <core/voxels/SparseVoxelOctree.h>
#include <assets_management/AssetsProvider.h>
#include <vulkan/vkUtils/VulkanGizmos.h>
#include <vulkan/vkUtils/VulkanMaterials.h>

namespace VoxelEngine
{
	SparseVoxelOctree* svo;
	renderer::mesh::QuadMesh editorGrid;
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
		editorGrid.vertexBuffer = VoxelEngine::renderer::VertexBuffer::Allocate(editorGrid.vertices.data(), editorGrid.vertexCount * sizeof(renderer::Vertex));
		editorGrid.indexBuffer = VoxelEngine::renderer::IndexBuffer::Allocate(editorGrid.indices.data(), editorGrid.indexCount * sizeof(uint32));
		editorGrid.material = utils::getMaterial("editor_grid");

		materials.solid = utils::getMaterial("solid_instanced");
		materials.wireframe = utils::getMaterial("wireframe_instanced");
		materials.normals = utils::getMaterial("normals");

		mesh = assets::AssetsProvider::loadObjMesh(ASSET_PATH("models/viking_room.obj"));

		svo = new SparseVoxelOctree(mesh, 1);
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
		editorGrid.material->bind();
		renderer::RenderCommand::drawMeshIndexed(editorGrid);
		
		switch (renderer::Renderer::getRenderSettings().renderMode)
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
				
		svo->traverse([&](OctreeNode& node)
		{
			utils::Gizmos::drawWireframeCube(node.box.min(), node.box.max());
		});
	}
}