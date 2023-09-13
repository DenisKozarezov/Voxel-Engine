#pragma once
#include <core/renderer/Renderer.h>
#include "../Timestep.h"

namespace VoxelEngine
{
	class Scene
	{
	public:
		std::vector<glm::vec3> vertices;

		Scene();
		~Scene();
		Scene(Scene const&) noexcept = delete;
		Scene(Scene&&) noexcept = delete;
		Scene& operator=(Scene const& rhs) noexcept = delete;
		Scene& operator=(Scene&& rhs) noexcept = delete;

		void update(const Timestep& ts, components::camera::Camera& camera);
		void renderScene();
	};
}