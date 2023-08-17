#pragma once
#include <glm/glm.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

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

namespace std
{
    template<> struct hash<VoxelEngine::renderer::Vertex>
    {
        size_t operator()(VoxelEngine::renderer::Vertex const& vertex) const
        {
            return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ (hash<glm::vec3>()(vertex.normal) << 1);
        }
    };
}