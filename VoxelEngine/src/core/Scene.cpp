#include "Scene.h"

namespace VoxelEngine
{
	Scene::Scene()
	{
		for (float x = -10.0f; x < 10.0f; x += 1.0f)
		{
			for (float y = -10.0f; y < 10.0f; y += 1.0f)
			{
				vertices.push_back({ x, y, 0.0f });
			}
		}
	}
}