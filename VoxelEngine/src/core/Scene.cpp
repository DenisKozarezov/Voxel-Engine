#include "Scene.h"

namespace VoxelEngine
{
	Scene::Scene()
	{
		for (int x = 0; x < 100; x += 5)
		{
			for (int y = 0; y < 100; y += 5)
			{
				for (int z = 0; z < 100; z += 5)
				{
					vertices.push_back({ x, y, z });
				}
			}
		}
	}
}