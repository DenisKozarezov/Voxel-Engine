#pragma once
#include "MeshData.h"

namespace marching
{
	static class Mesh {
	public:
		static MeshData build_mesh_data(std::vector<std::vector<std::vector<GridPoint>>> grid_points, glm::ivec3 grid_size, float surface, bool smooth_normals);
	};
}
