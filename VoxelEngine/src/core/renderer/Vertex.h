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

        constexpr Vertex() noexcept 
            : Vertex(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f))
        { }
        constexpr explicit Vertex(glm::vec3 position) noexcept 
            : Vertex(position, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f))
        { }
        constexpr explicit Vertex(glm::vec3 position, glm::vec3 normal) noexcept
            : Vertex(position, normal, glm::vec3(0.0f, 0.0f, 0.0f))
        { }
        constexpr explicit Vertex(glm::vec3 position, glm::vec3 normal, glm::vec3 color) noexcept
            : pos(position), normal(normal), color(color)
        { }

        constexpr bool operator==(const Vertex& other) const noexcept
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