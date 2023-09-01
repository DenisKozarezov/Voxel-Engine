#include "SparseVoxelOctree.h"
#include <cmath>

namespace VoxelEngine
{
	static int childCount = 0;

	SparseVoxelOctree::SparseVoxelOctree(int size, int maxDepth)
		: m_size(std::move(size)), m_maxDepth(std::move(maxDepth))
	{

	}

	uint32 SparseVoxelOctree::createChildDescriptor(OctreeNode* node, int& index, int pIndex)
	{
		unsigned int childDesc = 0;
		int ValidChildCount = 0;
		for (byte i = 0; i < 8; i++) 
		{
			if (OctreeNode* child = node->children[i]) 
			{
				childDesc |= 1 << i;
				if (child->isLeaf) 
				{
					childDesc |= BIT(i + 8);
					m_voxelCount++;
				}
				else 
				{
					if (!ValidChildCount) 
					{
						if ((index - pIndex) >= std::exp2(15)) 
						{
							childDesc |= BIT(16);
							m_Far.push_back(index - pIndex);
							childDesc |= m_Far.size() - 1 << 17;
						}
						else
							childDesc |= (index - pIndex) << 17;
					}
					ValidChildCount++;
				}
			}
		}

		childCount = ValidChildCount;
		index += ValidChildCount;
		return childDesc;
	}
	void SparseVoxelOctree::traverse(const std::function<void(OctreeNode*)>& visitor)
	{
		traverse(m_root, visitor);
	}

	void SparseVoxelOctree::traverse(OctreeNode* root, const std::function<void(OctreeNode*)>& visitor)
	{
		for (byte i = 0; i < 8; ++i) 
		{
			if (root->children[i] != nullptr) 
			{
				visitor(root->children[i]);
				traverse(root->children[i], visitor);
			}
		}
	}
	void SparseVoxelOctree::build()
	{
		int i = 0;
		build(m_root, i);
	}
	void SparseVoxelOctree::build(OctreeNode* node, int& index)
	{
		if (node == m_root)
			m_Buffer.push_back(createChildDescriptor(node, ++index, 0));

		int pIndex = index - childCount;
		m_Buffer.resize(index);
		for (byte i = 0; i < 8; i++) 
		{
			if (OctreeNode* child = node->children[i]) 
			{
				if (!child->isLeaf) 
				{
					m_Buffer.at(pIndex) = createChildDescriptor(child, index, pIndex);
					pIndex++;
					build(child, index);
				}
			}
		}

		/*int pIndex = index - childCount;
		traverse(node, [&](OctreeNode* node) {
			node->far = createChildDescriptor(node, index, pIndex);
		});*/
	}
	void SparseVoxelOctree::insert(OctreeNode** node, glm::vec3 point, glm::vec4 color, glm::ivec3 position, int depth)
	{
		if (*node == nullptr) 
		{
			*node = new OctreeNode;
		}

		if (depth >= m_maxDepth)
		{
			(*node)->data.color = color;
			(*node)->isLeaf = true;
			return;
		}

		(*node)->data.color = color;
		float size = (1.f / std::exp2(depth)) * m_size;
		glm::ivec3 childPos;

		childPos.x = (int)std::round((point.x - ((float)(position.x) * size)) / size);
		childPos.y = (int)std::round((point.y - ((float)(position.y) * size)) / size);
		childPos.z = (int)std::round((point.z - ((float)(position.z) * size)) / size);

		int childIndex = (childPos.x << 0) | (childPos.y << 1) | (childPos.z << 2);

		position = glm::vec3(
			(position.x << 1)	| childPos.x,
			(position.y << 1)	| childPos.y,
			(position.z << 1)	| childPos.z
		);
		(*node)->data.position = position;

		insert(&(*node)->children[childIndex], point, color, position, ++depth);
	}
	void SparseVoxelOctree::insert(glm::vec3 position, glm::vec4 color)
	{
		insert(&m_root, position, color, glm::ivec3(0), 0);
	}
}