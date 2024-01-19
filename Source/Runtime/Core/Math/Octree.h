#pragma once
#include "OctreeNode.h"
#include <functional>
#include <Engine/Components/mesh/Mesh.h>

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

		Box meshBounds(const TSharedPtr<components::mesh::Mesh>& mesh);
		std::array<Box, 8> getSubdividedOctants(OctreeNode* currentNode);
		const std::vector<glm::vec3> getMeshPointsInBox(
			const TSharedPtr<components::mesh::Mesh>& mesh,
			const Box& bounds);
		void subdivide(const TSharedPtr<components::mesh::Mesh>& mesh, OctreeNode* node, int level);
		void traverse(OctreeNode* root, const std::function<void(OctreeNode*)>& visitor);
	public:
		constexpr Octree();
		Octree(const TSharedPtr<components::mesh::Mesh>& mesh, int maxDepth);
		constexpr Octree(Box boundingBox, int maxDepth);
		~Octree();

		FORCE_INLINE const int& size() const { return m_size; }
		FORCE_INLINE const int& maxDepth() const { return m_maxDepth; }
		FORCE_INLINE const int& count() const { return m_voxelCount; }

		FORCE_INLINE void traverse(const std::function<void(OctreeNode*)>& visitor)
		{
			traverse(m_root, visitor);
		}
	};
}