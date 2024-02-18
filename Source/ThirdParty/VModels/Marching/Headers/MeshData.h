#pragma once
#include "../../Other/GridPoint.h"
#include <vector>

namespace marching
{
	struct MeshData final {
	public:
		std::vector<glm::vec3> verts;
		std::vector<glm::vec3> normals;
		std::vector<int> indices;

		MeshData() = default;
		MeshData(std::vector<glm::vec3> verts, std::vector<glm::vec3> normals, std::vector<int> indices) : verts(verts), normals(normals), indices(indices) { }

		~MeshData() = default;
	};
}