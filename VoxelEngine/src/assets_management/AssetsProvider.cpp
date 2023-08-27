#include "AssetsProvider.h"
#include <core/Assert.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

namespace assets
{
    const TextureData AssetsProvider::loadTexture(const string& path, ImageColorFormat req_comp)
    {
        TextureData data;
        
        data.m_nativePtr = stbi_load(path.c_str(), &data.m_width, &data.m_height, &data.m_texChannels, req_comp);

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
        VOXEL_CORE_ASSERT(isLoaded, warn + err);

        glm::mat4 preTransform = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        std::unordered_map<Vertex, uint32> uniqueVertices{};
        std::vector<Vertex> vertices;
        std::vector<uint32> indices;

        for (const auto& shape : shapes)
        {
            for (const auto& index : shape.mesh.indices)
            {
                glm::vec3 pos =
                {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
                };
                glm::vec3 normal =
                {
                    attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2],
                };

                Vertex vertex = Vertex(pos, normal);
                vertex.pos = glm::vec3(preTransform * glm::vec4(vertex.pos, 1.0f));
                vertex.normal = glm::vec3(preTransform * glm::vec4(vertex.normal, 1.0f));

                if (uniqueVertices.count(vertex) == 0)
                {
                    uniqueVertices[vertex] = static_cast<uint32>(vertices.size());
                    vertices.push_back(vertex);
                }
                indices.push_back(uniqueVertices[vertex]);
            }
        }

        uint32 vertexCount = static_cast<uint32>(vertices.size());
        uint32 indexCount = static_cast<uint32>(indices.size());

        VOXEL_CORE_TRACE("Finished to load OBJ mesh at path '{0}'.", path);
        VOXEL_CORE_TRACE("Loaded vertices count: '{0}'.", vertexCount);
        VOXEL_CORE_TRACE("Loaded indices count: '{0}'.", indexCount);

        return VoxelEngine::MakeShared<Mesh>(vertices.data(), vertexCount, indices.data(), indexCount);
    }
    void TextureData::release() const
    {
        stbi_image_free(m_nativePtr);
    }
}