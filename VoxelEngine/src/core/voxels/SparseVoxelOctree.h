#pragma once
#include <core/PrimitiveTypes.h>
#include <glm/glm.hpp>
#include <functional>
#include <stack>

namespace VoxelEngine
{
	struct VolumetricData
	{
		glm::vec3 position;
		glm::vec4 color;
	};

	struct OctreeNode
	{
		bool isLeaf;
		OctreeNode* children[8];
		VolumetricData data;

		constexpr OctreeNode()
		{
			isLeaf = false;
			data.position = glm::vec3(0.0f);
			data.color = glm::vec4(0.0f);
			for (int i = 0; i < 8; ++i)
			{
				children[i] = nullptr;
			}
		}
	};

	class SparseVoxelOctree
	{
	private:
		int m_size = 0;
		int m_maxDepth = 0;
		int m_voxelCount = 0;
		std::vector<unsigned int> m_Buffer, m_Far;
		std::vector<uint8_t> colors;
		OctreeNode* m_root = nullptr;

		uint32 createChildDescriptor(OctreeNode* node, int& index, int pIndex);
		bool intersects(const glm::vec3& voxelMin, const glm::vec3& voxelMax, const glm::vec3& nodeMin, const glm::vec3& nodeMax);
		void traverse(OctreeNode* root, const std::function<void(OctreeNode*)>& visitor);
	public:
		SparseVoxelOctree(int size, int maxDepth);

		inline constexpr int count() { return m_voxelCount; }
		void traverse(const std::function<void(OctreeNode*)>& visitor);
		void build();
		void build(OctreeNode* node, int& index);
		void insert(OctreeNode** node, glm::vec3 point, glm::vec4 color, glm::ivec3 position, int depth);
		void insert(glm::vec3 position, glm::vec4 color);
	};
}