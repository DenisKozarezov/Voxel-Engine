#pragma once
#include <entt/entt.hpp>
#include <core/renderer/Renderer.h>
#include "../Timestep.h"

namespace VoxelEngine
{
	class Scene
	{
	private:
		entt::registry m_registry;
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