#include "AssetsProvider.h"
#include <core/Assert.h>
#include <core/voxels/AABB.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

namespace assets
{
    using Vertex = VoxelEngine::renderer::Vertex;

    const TextureData AssetsProvider::loadTexture(const string& path, ImageColorFormat req_comp)
    {
        TextureData data;
        
        data.m_nativePtr = stbi_load(path.c_str(), &data.m_width, &data.m_height, &data.m_texChannels, req_comp);

        VOXEL_CORE_ASSERT(data.isValid(), "failed to load texture image on path '" + path + "'")

        return data;
    }
    const SharedRef<Mesh> AssetsProvider::loadObjMesh(const string& path)
    {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        VOXEL_CORE_TRACE("Loading OBJ mesh at path '{0}'...", path);

        bool isLoaded = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str());
        VOXEL_CORE_ASSERT(isLoaded, warn + err);

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

        VOXEL_CORE_TRACE("Finished to load OBJ mesh at path '{0}'.", path);
        VOXEL_CORE_TRACE("Loaded vertices count: '{0}'.", vertexCount);
        VOXEL_CORE_TRACE("Loaded indices count: '{0}'.", indexCount);

        return MakeShared<Mesh>(vertices, indices);
    }
    const SharedRef<VoxelEngine::SparseVoxelOctree> AssetsProvider::loadObjVoxelizedMesh(const string& path, const int& size, const int& depth)
    {
        std::ifstream fp(path);
        std::string line;
        VOXEL_CORE_ASSERT(fp, "failed to load obj file at path" + path);

        std::vector<glm::vec3> positions;
        std::vector<glm::ivec3> faces;

        glm::vec3 modelMax{0};
        glm::vec3 modelMin{100000.f};

        while (std::getline(fp, line)) {
            if (line.rfind("v ") == 0) {
                line.erase(0, line.find(' ') + 1);
                glm::vec3 pos;
                for (int i = 0; i < 3; i++) {
                    int index = line.find(' ');
                    pos[i] = std::stof(line.substr(0, index));
                    if (pos[i] > modelMax[i]) modelMax[i] = pos[i];
                    if (pos[i] < modelMin[i]) modelMin[i] = pos[i];
                    line.erase(0, index + 1);
                }
                positions.push_back(pos);
            }
            if (line.rfind("f ") == 0) {
                line.erase(0, line.find(' ') + 1);
                glm::ivec3 f;
                for (int i = 0; i < 3; i++) {
                    for (int j = 0; j < 3; j++) {
                        int index = line.find('/') < line.find(' ') ? line.find('/') : line.find(' ');
                        if (j == 0)
                            f[i] = std::stoi(line.substr(0, index));
                        else
                            std::stoi(line.substr(0, index));
                        line.erase(0, index + 1);
                    }
                }
                faces.push_back(f);
            }
        }

        VoxelEngine::SparseVoxelOctree octree(size, depth);
        float min = std::min(std::min(modelMin.x, modelMin.y), modelMin.z);
        float max = std::max(std::max(modelMax.x, modelMax.y), modelMax.z);

        for (auto face : faces) 
        {
            glm::vec3 triangle[3];
            for (int i = 0; i < 3; i++) 
            {
                triangle[i] = {
                    ((positions[face[i] - 1].x + std::abs(min)) / (std::abs(min) + max))* size,
                    ((positions[face[i] - 1].y + std::abs(min)) / (std::abs(min) + max)) * size,
                    ((positions[face[i] - 1].z + std::abs(min)) / (std::abs(min) + max)) * size,
                };
            }
            for (glm::vec3 point : voxelize(triangle, size, depth)) 
            {
                octree.insert(point, {255, 255, 255, 255});
            }
        }

        return MakeShared<VoxelEngine::SparseVoxelOctree>(octree);
    }
    const std::vector<glm::vec3> AssetsProvider::voxelize(glm::vec3* triangle, const int& size, const int& depth)
    {
        glm::vec3 max = { 0, 0, 0 }, min{ 1000, 1000, 1000 };

        for (int i = 0; i < 3; i++) 
        {
            for (int j = 0; j < 3; j++) 
            {
                if (triangle[i][j] < min[j]) min[j] = triangle[i][j];
                if (triangle[i][j] > max[j]) max[j] = triangle[i][j];
            }
        }

        float s = std::exp2(-depth) * size;

        max = { std::ceil(max.x / s) * s, std::ceil(max.y / s) * s, std::ceil(max.z / s) * s };
        min = { std::floor(min.x / s) * s, std::floor(min.y / s) * s, std::floor(min.z / s) * s };
        std::vector<glm::vec3> overlapPoints;

        for (int z = (int)(min.z / s); z <= (int)(max.z / s); z++) 
        {
            for (int y = (int)(min.y / s); y <= (int)(max.y / s); y++) 
            {
                for (int x = (int)(min.x / s); x <= (int)(max.x / s); x++) 
                {
                    glm::vec3 halfWidth = { s / 2.f, s / 2.f, s / 2.f };
                    glm::vec3 center = glm::vec3{ x * s, y * s, z * s } +
                        glm::vec3{
                            x == (int)(max.x / s) ? -halfWidth.x : halfWidth.x,
                            y == (int)(max.y / s) ? -halfWidth.y : halfWidth.y,
                            z == (int)(max.z / s) ? -halfWidth.z : halfWidth.z,
                    };
                    if (triBoxOverlap(center, halfWidth, triangle)) 
                    {
                        overlapPoints.push_back(center);
                    }
                }
            }
        }

        return overlapPoints;
    }
    void TextureData::release() const
    {
        stbi_image_free(m_nativePtr);
    }
}