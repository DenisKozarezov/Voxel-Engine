#pragma once
#include <vector>
#include <core/voxels/Box.h>

namespace VoxelEngine
{
	struct VolumetricData
	{
		glm::vec3 position;
		glm::vec4 color;
	};

	struct OctreeNode
	{
		std::vector<OctreeNode*> children;
		std::vector<glm::vec3> points;
		Box box;

		constexpr OctreeNode() noexcept = default;
		constexpr OctreeNode(const OctreeNode& node)
		{
			box = node.box;
			std::copy(node.children.begin(), node.children.end(), std::back_inserter(children));
			std::copy(node.points.begin(), node.points.end(), std::back_inserter(points));
		}
		constexpr OctreeNode(OctreeNode&& node) noexcept
		{
			box = std::move(node.box);
			children.swap(node.children);
			points.swap(node.points);
		}
		~OctreeNode()
		{
			for (const OctreeNode* node : children)
				delete node;
		}

		INLINE const bool isLeaf() const
		{
			return children.size() == 0;
		}
	};
}