#include "Scene.h"

namespace VoxelEngine
{
	Scene::Scene()
	{
		for (int x = 0; x < 100; x++)
		{
			for (int y = 0; y < 100; y++)
			{
				vertices.push_back({ x, y, 0 });
			}
		}
	}
}