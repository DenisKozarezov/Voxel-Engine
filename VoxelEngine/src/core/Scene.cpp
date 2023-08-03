#include "Scene.h"

namespace VoxelEngine
{
	Scene::Scene()
	{
		for (int x = 0; x < 100; x++)
		{
			for (int y = 0; y < 100; y++)
			{
				for (int z = 0; z < 100; z++)
				{
					vertices.push_back({ x, y, z });
				}
			}
		}
	}
}