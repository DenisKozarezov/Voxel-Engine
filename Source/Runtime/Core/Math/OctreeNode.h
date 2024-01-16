#pragma once
#include <vector>
#include "Box.h"

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
		Box bounds;

		constexpr OctreeNode(Box bounds) noexcept
		{
			this->bounds = bounds;
		}
		constexpr OctreeNode(const OctreeNode& node)
		{
			bounds = node.bounds;
			children.assign(node.children.begin(), node.children.end());
			points.assign(node.points.begin(), node.points.end());
		}
		constexpr OctreeNode(OctreeNode&& node) noexcept
		{
			bounds = node.bounds;
			children.swap(node.children);
			points.swap(node.points);
		}
		~OctreeNode()
		{
			for (const OctreeNode* node : children)
				delete node;
		}

		FORCE_INLINE constexpr bool isLeaf() { return children.empty(); }
		FORCE_INLINE constexpr bool isLeaf() const { return children.empty(); }
	};
}