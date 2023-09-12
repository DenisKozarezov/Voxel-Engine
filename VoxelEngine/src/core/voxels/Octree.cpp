#include "Octree.h"

namespace VoxelEngine
{
	constexpr Octree::Octree() : Octree(Box(), 1) { }
	Octree::Octree(const SharedRef<components::mesh::Mesh>& mesh, int maxDepth)
		: m_maxDepth(maxDepth)
	{
		VOXEL_CORE_ASSERT(maxDepth > 0, "Octree's max depth must be greater than zero!");

		Box boundingBox = meshBounds(mesh);
		m_root = new OctreeNode();
		m_root->box = boundingBox;
		subdivide(mesh, m_root, 0);
	}
	constexpr Octree::Octree(Box boundingBox, int maxDepth)
		: m_maxDepth(maxDepth)
	{
		VOXEL_CORE_ASSERT(maxDepth > 0, "Octree's max depth must be greater than zero!");

		m_root = new OctreeNode();
		m_root->box = boundingBox;
	}

	Octree::~Octree()
	{
		delete m_root;
	}

	const std::vector<glm::vec3> Octree::getMeshPointsInBox(
		const SharedRef<components::mesh::Mesh>& mesh,
		const Box& box)
	{
		std::vector<glm::vec3> result;
		for (int i = 0; i < mesh->vertexCount; i++)
		{
			glm::vec3 pos = mesh->vertices[i].pos;
			if (box.inside(pos))
			{
				result.push_back(pos);
			}
		}
		return result;
	}
	void Octree::traverse(const std::function<void(OctreeNode*)>& visitor)
	{
		traverse(m_root, visitor);
	}
	Box Octree::meshBounds(const SharedRef<components::mesh::Mesh>& mesh)
	{
		if (mesh->vertexCount == 0)
			return Box();

		glm::vec3 min = glm::vec3(std::numeric_limits<float>::max());
		glm::vec3 max = glm::vec3(-std::numeric_limits<float>::max());

		for (uint32 i = 0; i < mesh->vertexCount; i++) 
		{
			glm::vec3 vertex = mesh->vertices[i].pos;
			min = glm::min(min, vertex);
			max = glm::max(max, vertex);
		}
		return Box(min, max);
	}
	void Octree::subdivide(const SharedRef<components::mesh::Mesh>& mesh, OctreeNode* node, int level)
	{
		level++;

		if (level > m_maxDepth)
			return;

		node->children.resize(8);
		std::array<Box, 8> listBox = getSubdividedOctants(node);
		for (int i = 0; i < 8; i++)
		{
			OctreeNode* child = new OctreeNode;
			child->box = listBox[i];

			std::vector<glm::vec3> points = getMeshPointsInBox(mesh, child->box);
			for (auto& p : points)
			{
				child->points.push_back(p);
			}

			subdivide(mesh, child, level);

			if (child->points.size() > 0)
			{
				m_voxelCount++;			
				node->children[i] = child;
			}
		}
	}
	constexpr std::array<Box, 8> Octree::getSubdividedOctants(OctreeNode* currentNode)
	{
		glm::vec3 min = currentNode->box.min();
		glm::vec3 max = currentNode->box.max();
		glm::vec3 center = currentNode->box.center();

		std::array<Box, 8> octant;
		octant[0] = (currentNode->children[0] != nullptr) ? currentNode->children[0]->box : Box(min, center);
		octant[1] = (currentNode->children[1] != nullptr) ? currentNode->children[1]->box : Box(glm::vec3(center.x, min.y, min.z), glm::vec3(max.x, center.y, center.z));
		octant[2] = (currentNode->children[2] != nullptr) ? currentNode->children[2]->box : Box(glm::vec3(center.x, min.y, center.z), glm::vec3(max.x, center.y, max.z));
		octant[3] = (currentNode->children[3] != nullptr) ? currentNode->children[3]->box : Box(glm::vec3(min.x, min.y, center.z), glm::vec3(center.x, center.y, max.z));
		octant[4] = (currentNode->children[4] != nullptr) ? currentNode->children[4]->box : Box(glm::vec3(min.x, center.y, min.z), glm::vec3(center.x, max.y, center.z));
		octant[5] = (currentNode->children[5] != nullptr) ? currentNode->children[5]->box : Box(glm::vec3(center.x, center.y, min.z), glm::vec3(max.x, max.y, center.z));
		octant[6] = (currentNode->children[6] != nullptr) ? currentNode->children[6]->box : Box(center, max);
		octant[7] = (currentNode->children[7] != nullptr) ? currentNode->children[7]->box : Box(glm::vec3(min.x, center.y, center.z), glm::vec3(center.x, max.y, max.z));
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