#pragma once
#include <vector>
#include <glm/vec3.hpp>

namespace marching
{
    class VoxelArray {
    private:
        static float b_lerp(float v00, float v10, float v01, float v11, float tx, float ty);
    public:
        bool flip_normals;
        std::vector<std::vector<std::vector<float>>> voxels;
        VoxelArray(int width, int height, int depth) : flip_normals(true) { 
            voxels.resize(width);
            for (int i = 0; i < width; i++)
            {
                voxels[i].resize(height);
                for (int j = 0; j < height; j++)
                    voxels[i][j].resize(depth);
            }
        }
        ~VoxelArray() = default;

        int width() {
            return voxels.size();
        }
        int height() const {
            return voxels[0].size();
        }
        int depth() const {
            return voxels[0][0].size();
        }

        float get_voxel(int x, int y, int z);
        float get_voxel(float u, float v, float w);
        glm::vec3 get_normal(int x, int y, int z);
        glm::vec3 get_normal(float u, float v, float w);
        glm::vec3 get_first_derivative(int x, int y, int z);
        glm::vec3 get_first_derivative(float u, float v, float w);
    };
}