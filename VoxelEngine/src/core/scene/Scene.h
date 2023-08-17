#pragma once
#include <vector>
#include <glm/vec3.hpp>
#include "../Timestep.h"

namespace VoxelEngine
{
	class Scene
	{
	public:
		std::vector<glm::vec3> vertices;

		Scene();
		~Scene() noexcept = default;
		Scene(Scene const&) noexcept = delete;
		Scene(Scene&&) noexcept = delete;
		Scene& operator=(Scene const& rhs) noexcept = delete;
		Scene& operator=(Scene&& rhs) noexcept = delete;

		void update(const Timestep& ts);
	};
}