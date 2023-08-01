#pragma once
#include <vector>
#include <glm/vec3.hpp>

namespace VoxelEngine
{
	class Scene
	{
	public:
		std::vector<glm::vec3> vertices;

		Scene();
	};
}