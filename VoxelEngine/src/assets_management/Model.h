#pragma once
#include <vector>
#include "components/mesh/Mesh.h"

namespace assets
{
	struct Model
	{
		using Mesh = VoxelEngine::components::mesh::Mesh;

		Mesh mesh;
		std::vector<string> textures;

		explicit Model(
			VoxelEngine::components::mesh::Mesh& mesh,
			const std::vector<std::string>& textures);

		Model(Model const&) noexcept = delete;
		Model(Model&&) noexcept = delete;
		Model& operator= (Model const& rhs) noexcept = delete;
		Model& operator= (Model&& rhs) noexcept = delete;

		~Model() noexcept = default;
	};
}