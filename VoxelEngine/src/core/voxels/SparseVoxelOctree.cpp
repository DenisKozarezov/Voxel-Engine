#include "SparseVoxelOctree.h"
#include <cmath>

namespace VoxelEngine
{
	std::vector<int> convertVectorIndicesToInts(const uint32* indices, uint32 indexCount) 
	{
		std::vector<int> intVector(indexCount);

		for (int i = 0; i < indexCount; i++)
		{
			intVector[i] = indices[i];
		}
		return intVector;
	}


	SparseVoxelOctree::SparseVoxelOctree(const SharedRef<components::mesh::Mesh>& mesh, int maxDepth)
		: m_maxDepth(std::move(maxDepth))
	{
		m_root = OctreeNode();

		Box boundingBox = meshBounds(mesh);
		m_root.box = boundingBox;

		//intialize root.points
		//std::vector<int> pointsInsideMesh(mesh->indexCount);
		//std::vector<int> pointsInsideBox;
		////convert ofIndexType vector to int vector
		//pointsInsideMesh = convertVectorIndicesToInts(mesh->indices, mesh->indexCount);
		//int totalPointsInBox = getMeshPointsInBox(mesh, pointsInsideMesh, boundingBox, pointsInsideBox);

		//m_root.points = pointsInsideBox;

		subdivide(mesh, m_root, 0);
	}
	SparseVoxelOctree::~SparseVoxelOctree()
	{

	}

	int SparseVoxelOctree::getMeshPointsInBox(
		const SharedRef<components::mesh::Mesh>& mesh,
		const std::vector<int>& points,
		Box& box,
		std::vector<int>& pointsRtn)
	{
		int count = 0;
		for (int i = 0; i < points.size(); i++) 
		{
			glm::vec3 pos = mesh->vertices[points[i]].pos;
			if (box.inside(pos))
			{
				count++;
				pointsRtn.push_back(points[i]);
			}
		}
		return count;
	}
	void SparseVoxelOctree::traverse(const std::function<void(OctreeNode&)>& visitor)
	{
		traverse(m_root, visitor);
	}
	Box SparseVoxelOctree::meshBounds(const SharedRef<components::mesh::Mesh>& mesh)
	{
		if (mesh->vertexCount == 0)
			return Box();

		glm::vec3 vertex = mesh->vertices[0].pos;
		glm::vec3 max = vertex;
		glm::vec3 min = vertex;
		for (int i = 1; i < mesh->vertexCount; i++)
		{
			vertex = mesh->vertices[i].pos;

			if (vertex.x > max.x)       max.x = vertex.x;
			else if (vertex.x < min.x)  min.x = vertex.x;

			if (vertex.y > max.y)       max.y = vertex.y;
			else if (vertex.y < min.y)  min.y = vertex.y;

			if (vertex.z > max.z)       max.z = vertex.z;
			else if (vertex.z < min.z)  min.z = vertex.z;
		}
		return Box(min, max);
	}
	void SparseVoxelOctree::subdivide(const SharedRef<components::mesh::Mesh>& mesh, OctreeNode& node, int level)
	{
		level++;		

		if (level > m_maxDepth)
			return;

		//divide the node.box by 8 and put each box into a boxList
		std::vector<Box> boxList;
		subDivideBox8(node.box, boxList);

		//create 8 nodes and attach it to that root.children
		for (int i = 0; i < 8; i++)
		{
			OctreeNode child = OctreeNode();

			child.box = boxList[i];

			//getMeshPointsInBox(mesh, node.points, child.box, child.points);

			subdivide(mesh, child, level);

			node.children.push_back(child);

			m_voxelCount++;
		}
	}

	void SparseVoxelOctree::subDivideBox8(const Box& box, std::vector<Box>& boxList)
	{
		glm::vec3 min = box.min();
		glm::vec3 max = box.max();
		glm::vec3 center = box.center();
		glm::vec3 bounds = max - min;
		float xdist = (max.x - min.x) * 0.5f;
		float ydist = (max.y - min.y) * 0.5f;
		float zdist = (max.z - min.z) * 0.5f;
		glm::vec3 h = glm::vec3(0.0f, ydist, 0.0f);

		//  generate ground floor
		//
		Box b[8];
		b[0] = Box(min, center);
		b[1] = Box(b[0].min() + glm::vec3(xdist, 0, 0), b[0].max() + glm::vec3(xdist, 0, 0));
		b[2] = Box(b[1].min() + glm::vec3(0, 0, zdist), b[1].max() + glm::vec3(0, 0, zdist));
		b[3] = Box(b[2].min() + glm::vec3(-xdist, 0, 0), b[2].max() + glm::vec3(-xdist, 0, 0));

		boxList.clear();
		for (int i = 0; i < 4; i++)
			boxList.push_back(b[i]);

		// generate second story
		//
		for (int i = 4; i < 8; i++) {
			b[i] = Box(b[i - 4].min() + h, b[i - 4].max() + h);
			boxList.push_back(b[i]);
		}
	}

	void SparseVoxelOctree::traverse(OctreeNode& root, const std::function<void(OctreeNode&)>& visitor)
	{
		for (byte i = 0; i < 8; ++i) 
		{
			if (root.children.size() > 0) 
			{
				visitor(root.children[i]);
				traverse(root.children[i], visitor);
			}
		}
	}
}