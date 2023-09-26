#pragma once
#include <functional>
#include <core/voxels/OctreeNode.h>
#include <components/mesh/Mesh.h>

namespace VoxelEngine
{
	class Octree
	{
	private:
		int m_size = 0;
		int m_maxDepth = 0;
		int m_voxelCount = 0;
		OctreeNode* m_root = nullptr;

		static constexpr int MIN_SIZE = 1;

		Box meshBounds(const SharedRef<components::mesh::Mesh>& mesh);
		const std::vector<glm::vec3> getMeshPointsInBox(
			const SharedRef<components::mesh::Mesh>& mesh,
			const Box& bounds);
		void subdivide(const SharedRef<components::mesh::Mesh>& mesh, OctreeNode* node, int level);
		constexpr std::array<Box, 8> getSubdividedOctants(OctreeNode* currentNode);
		void traverse(OctreeNode* root, const std::function<void(OctreeNode*)>& visitor);
	public:
		constexpr Octree();
		Octree(const SharedRef<components::mesh::Mesh>& mesh, int maxDepth);
		constexpr Octree(Box boundingBox, int maxDepth);
		~Octree();

		INLINE const int& size() { return m_size; }
		INLINE const int& maxDepth() { return m_maxDepth; }
		INLINE const int& count() { return m_voxelCount; }

		INLINE void traverse(const std::function<void(OctreeNode*)>& visitor)
		{
			traverse(m_root, visitor);
		}
	};
}