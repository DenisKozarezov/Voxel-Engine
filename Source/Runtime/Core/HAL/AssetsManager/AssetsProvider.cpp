#include "AssetsProvider.h"
#include <Core/Logging/Assert.h>

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

        RUNTIME_ASSERT(data.isValid(), "failed to load texture image on path '" + path + "'");

        return data;
    }
    const TSharedPtr<Mesh> AssetsProvider::loadObjMesh(const string& path)
    {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        RUNTIME_TRACE("Loading OBJ mesh at path '{0}'...", path);

        const bool isLoaded = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str());
        RUNTIME_ASSERT(isLoaded, warn + err);
        RUNTIME_ASSERT(!attrib.normals.empty(), "there are no 'vn' definitions (for normals) in the .obj file! Check the file format!");

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

                glm::vec3 normal = {
                    attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2],
                };
                glm::vec3 color = { 1.0f, 0.0f, 0.0f };
                Vertex vertex = Vertex(pos, normal, color);

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

        auto mesh = MakeShared<Mesh>(vertices, indices);
        mesh->vertexBuffer = VoxelEngine::renderer::VertexBuffer::Allocate(vertices.data(), vertexCount * sizeof(Vertex));
        mesh->indexBuffer = VoxelEngine::renderer::IndexBuffer::Allocate(indices.data(), indexCount * sizeof(uint32));

        RUNTIME_TRACE("Finished to load OBJ mesh at path '{0}'.", path);
        RUNTIME_TRACE("Loaded vertices count: '{0}'.", vertexCount);
        RUNTIME_TRACE("Loaded indices count: '{0}'.", indexCount);

        return mesh;
    }

    void TextureData::release() const
    {
        stbi_image_free(m_nativePtr);
    }
}