#include "AssetsProvider.h"
#include <core/Assert.h>
#include <vulkan/VulkanVertex.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

namespace std {
    template<> struct hash<vulkan::Vertex> 
    {
        size_t operator()(vulkan::Vertex const& vertex) const 
        {
            return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.texCoord) << 1);
        }
    };
}

namespace assets
{
    const VoxelEngine::SharedRef<Mesh*> AssetsProvider::loadObjMesh(const string& path)
    {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        bool isLoaded = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str());
        VOXEL_ASSERT(isLoaded, warn + err)
        
        std::unordered_map<vulkan::Vertex, uint32> uniqueVertices{};
        Mesh* mesh = new Mesh;

        for (const auto& shape : shapes)
        {
            for (const auto& index : shape.mesh.indices)
            {
                vulkan::Vertex vertex =
                {
                    .pos =
                    {
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2]
                    },
                    .color = { 1.0f, 1.0f, 1.0f },
                    .texCoord =
                    {
                        attrib.texcoords[2 * index.texcoord_index + 0],
                        1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
                    }
                };

                if (uniqueVertices.count(vertex) == 0) 
                {
                    uniqueVertices[vertex] = static_cast<uint32>(mesh->vertices.size());
                    mesh->vertices.push_back(vertex);
                }
                mesh->indices.push_back(uniqueVertices[vertex]);
            }
        }

        return VoxelEngine::MakeShared<Mesh*>(mesh);
    }
}