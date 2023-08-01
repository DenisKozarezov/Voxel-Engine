#include "AssetsProvider.h"
#include <core/Assert.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

using Vertex = VoxelEngine::renderer::Vertex;

namespace std 
{

    template<> struct hash<Vertex>
    {
        size_t operator()(Vertex const& vertex) const
        {
            return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ (hash<glm::vec3>()(vertex.normal) << 1);
        }
    };
}

namespace assets
{
    const TextureData AssetsProvider::loadTexture(const string& path, ImageColorFormat req_comp)
    {
        TextureData data;
        
        data.nativePtr = stbi_load(path.c_str(), &data.width, &data.height, &data.texChannels, req_comp);

        VOXEL_CORE_ASSERT(data.isValid(), "failed to load texture image on path '" + path + "'")

        return data;
    }
    const VoxelEngine::SharedRef<Mesh> AssetsProvider::loadObjMesh(const string& path)
    {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        VOXEL_CORE_TRACE("Loading OBJ mesh at path '{0}'...", path);

        bool isLoaded = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str());
        VOXEL_CORE_ASSERT(isLoaded, warn + err)

        Mesh* mesh = new Mesh;
        glm::mat4 preTransform = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        std::unordered_map<Vertex, uint32> uniqueVertices{};

        for (const auto& shape : shapes)
        {
            for (const auto& index : shape.mesh.indices)
            {
                Vertex vertex =
                {
                    .pos =
                    {
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2]
                    },
                    .normal =
                    {
                        attrib.normals[3 * index.normal_index + 0],
                        attrib.normals[3 * index.normal_index + 1],
                        attrib.normals[3 * index.normal_index + 2],
                    }
                };
                vertex.pos = glm::vec3(preTransform * glm::vec4(vertex.pos, 1.0f));
                vertex.normal = glm::vec3(preTransform * glm::vec4(vertex.normal, 1.0f));

                if (uniqueVertices.count(vertex) == 0)
                {
                    uniqueVertices[vertex] = static_cast<uint32>(mesh->vertices.size());
                    mesh->vertices.push_back(vertex);
                }
                mesh->indices.push_back(uniqueVertices[vertex]);
            }
        }

        VOXEL_CORE_TRACE("Finished to load OBJ mesh at path '{0}'.", path);

        return VoxelEngine::MakeShared<Mesh>(*mesh);
    }
    void TextureData::release() const
    {
        stbi_image_free(nativePtr);
    }
}