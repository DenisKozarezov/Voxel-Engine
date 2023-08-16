#include "Scene.h"

namespace VoxelEngine
{
	Scene::Scene()
	{
		const float s = 0.1f * 2;
		const int N = 5;

		for (float x = s; x < s * N; x += s)
		{
			for (float y = 0; y < s * N; y += s)
			{
				for (float z = y; z < s * N; z += s)
				{
					vertices.push_back({ x, y + s, z });
				}
			}
		}
	}

	void Scene::update(const Timestep& ts)
	{

	}
}