#include "Model.h"

namespace assets
{
	Model::Model(VoxelEngine::components::mesh::Mesh& mesh, const std::vector<std::string>& textures)
		: mesh(std::move(mesh)), textures(std::move(textures))
	{ }
}