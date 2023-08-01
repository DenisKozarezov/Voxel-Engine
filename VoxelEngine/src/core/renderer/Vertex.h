#pragma once
#include <glm/glm.hpp>

namespace VoxelEngine::renderer
{
	struct Vertex
	{
        glm::vec3 pos;
        glm::vec3 normal;
        glm::vec3 color;

        const bool operator==(const Vertex& other) const 
        {
            return pos == other.pos && color == other.color && normal == other.normal;
        }
	};
}