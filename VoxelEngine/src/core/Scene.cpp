#include "Scene.h"

namespace VoxelEngine
{
	Scene::Scene()
	{
		for (int x = 0; x < 5; x++)
		{
			for (int y = 0; y < 5; y++)
			{
				vertices.push_back({ x, y, 0 });
			}
		}
	}
}