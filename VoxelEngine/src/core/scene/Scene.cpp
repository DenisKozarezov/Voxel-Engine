#include "Scene.h"

namespace VoxelEngine
{
	Scene::Scene()
	{
		for (float x = 0; x < 1; x += 0.2f)
		{
			for (float y = 0; y < 1; y += 0.2f)
			{
				for (float z = y; z < 1; z += 0.2f)
				{
					vertices.push_back({ x, y + 0.2f, z });
				}
			}
		}
	}
}