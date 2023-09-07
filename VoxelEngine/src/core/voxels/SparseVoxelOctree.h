#pragma once
#include <core/PrimitiveTypes.h>
#include <glm/glm.hpp>
#include <functional>
#include <stack>
#include <core/voxels/Box.h>
#include <components/mesh/Mesh.h>

namespace VoxelEngine
{
	struct VolumetricData
	{
		glm::vec3 position;
		glm::vec4 color;
	};

	struct OctreeNode
	{
		std::vector<OctreeNode> children;
		std::vector<int> points;
		Box box;

		OctreeNode() = default;
	};

	class SparseVoxelOctree
	{
	private:
		int m_size = 0;
		int m_maxDepth = 0;
		int m_voxelCount = 0;
		OctreeNode m_root;

		Box meshBounds(const SharedRef<components::mesh::Mesh>& mesh);
		int getMeshPointsInBox(
			const SharedRef<components::mesh::Mesh>& mesh,
			const std::vector<int>& points,
			Box& box,
			std::vector<int>& pointsRtn);
		void subdivide(const SharedRef<components::mesh::Mesh>& mesh, OctreeNode& node, int level);
		void subDivideBox8(const Box& box, std::vector<Box>& boxList);
		void traverse(OctreeNode& root, const std::function<void(OctreeNode&)>& visitor);
	public:
		SparseVoxelOctree(const SharedRef<components::mesh::Mesh>& mesh, int maxDepth);
		~SparseVoxelOctree();

		INLINE const int& size() { return m_size; }
		INLINE const int& maxDepth() { return m_maxDepth; }
		INLINE const int& count() { return m_voxelCount; }

		void traverse(const std::function<void(OctreeNode&)>& visitor);
	};
}