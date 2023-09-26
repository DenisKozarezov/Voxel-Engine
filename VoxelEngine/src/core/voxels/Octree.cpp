#include "Octree.h"
#include <array>

namespace VoxelEngine
{
	constexpr Octree::Octree() : Octree(Box(), 1) { }
	Octree::Octree(const SharedRef<components::mesh::Mesh>& mesh, int maxDepth)
		: m_maxDepth(maxDepth)
	{
		VOXEL_CORE_ASSERT(maxDepth > 0, "Octree's max depth must be greater than zero!");

		Box boundingBox = meshBounds(mesh);
		m_root = new OctreeNode(boundingBox);
		subdivide(mesh, m_root, 0);
	}
	constexpr Octree::Octree(Box boundingBox, int maxDepth)
		: m_maxDepth(maxDepth)
	{
		VOXEL_CORE_ASSERT(maxDepth > 0, "Octree's max depth must be greater than zero!");

		m_root = new OctreeNode(boundingBox);
	}

	Octree::~Octree()
	{
		delete m_root;
	}

	const std::vector<glm::vec3> Octree::getMeshPointsInBox(
		const SharedRef<components::mesh::Mesh>& mesh,
		const Box& bounds)
	{
		std::vector<glm::vec3> result;
		for (int i = 0; i < mesh->vertexCount(); ++i)
		{
			glm::vec3 pos = mesh->vertices[i].pos;
			if (bounds.inside(pos))
			{
				result.push_back(pos);
			}
		}
		return result;
	}
	Box Octree::meshBounds(const SharedRef<components::mesh::Mesh>& mesh)
	{
		if (mesh->vertexCount() == 0)
			return Box();

		glm::vec3 min = glm::vec3(std::numeric_limits<float>::max());
		glm::vec3 max = glm::vec3(-std::numeric_limits<float>::max());

		for (uint32 i = 0; i < mesh->vertexCount(); ++i) 
		{
			glm::vec3 vertex = mesh->vertices[i].pos;
			min = glm::min(min, vertex);
			max = glm::max(max, vertex);
		}
		return Box(min, max);
	}
	void Octree::subdivide(const SharedRef<components::mesh::Mesh>& mesh, OctreeNode* node, int level)
	{
		++level;

		if (level > m_maxDepth)
			return;

		node->children.resize(8);
		std::array<Box, 8> listBox = getSubdividedOctants(node);
		for (int i = 0; i < 8; ++i)
		{
			OctreeNode* child = new OctreeNode(listBox[i]);

			std::vector<glm::vec3> points = getMeshPointsInBox(mesh, child->bounds);
			for (auto& p : points)
			{
				child->points.push_back(p);
			}

			subdivide(mesh, child, level);

			if (child->points.size() > 0)
			{
				++m_voxelCount;			
				node->children[i] = child;
			}
		}
	}
	constexpr std::array<Box, 8> Octree::getSubdividedOctants(OctreeNode* currentNode)
	{
		glm::vec3 min = currentNode->bounds.min();
		glm::vec3 max = currentNode->bounds.max();
		glm::vec3 center = currentNode->bounds.center();

		std::array<Box, 8> octant;
		octant[0] = (currentNode->children[0] != nullptr) ? currentNode->children[0]->bounds : Box(min, center);
		octant[1] = (currentNode->children[1] != nullptr) ? currentNode->children[1]->bounds : Box({ center.x, min.y, min.z },		{ max.x, center.y, center.z });
		octant[2] = (currentNode->children[2] != nullptr) ? currentNode->children[2]->bounds : Box({ center.x, min.y, center.z },	{ max.x, center.y, max.z });
		octant[3] = (currentNode->children[3] != nullptr) ? currentNode->children[3]->bounds : Box({ min.x, min.y, center.z },		{ center.x, center.y, max.z });
		octant[4] = (currentNode->children[4] != nullptr) ? currentNode->children[4]->bounds : Box({ min.x, center.y, min.z },		{ center.x, max.y, center.z });
		octant[5] = (currentNode->children[5] != nullptr) ? currentNode->children[5]->bounds : Box({ center.x, center.y, min.z },	{ max.x, max.y, center.z });
		octant[6] = (currentNode->children[6] != nullptr) ? currentNode->children[6]->bounds : Box(center, max);
		octant[7] = (currentNode->children[7] != nullptr) ? currentNode->children[7]->bounds : Box({ min.x, center.y, center.z },	{ center.x, max.y, max.z });
		return octant;
	}

	void Octree::traverse(OctreeNode* root, const std::function<void(OctreeNode*)>& visitor)
	{
		for (OctreeNode* node : root->children)
		{
			if (node == nullptr)
				continue;

			visitor(node);
			traverse(node, visitor);
		}
	}
}