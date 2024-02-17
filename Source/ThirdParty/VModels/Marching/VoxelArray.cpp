#include "./Headers/VoxelArray.h"
#include <algorithm>
#include <cmath>
#include <glm/geometric.hpp>

namespace marching
{
    float VoxelArray::get_voxel(int x, int y, int z) {
        x = std::clamp(x, 0, width() - 1);
        y = std::clamp(y, 0, height() - 1);
        z = std::clamp(z, 0, depth() - 1);
        return voxels[x][y][z];
    }

    float VoxelArray::get_voxel(float u, float v, float w) {
        float x = u * (width() - 1);
        float y = v * (height() - 1);
        float z = w * (depth() - 1);

        int xi = static_cast<int>(std::floor(x));
        int yi = static_cast<int>(std::floor(y));
        int zi = static_cast<int>(std::floor(z));

        float v000 = get_voxel(xi, yi, zi);
        float v100 = get_voxel(xi + 1, yi, zi);
        float v010 = get_voxel(xi, yi + 1, zi);
        float v110 = get_voxel(xi + 1, yi + 1, zi);

        float v001 = get_voxel(xi, yi, zi + 1);
        float v101 = get_voxel(xi + 1, yi, zi + 1);
        float v011 = get_voxel(xi, yi + 1, zi + 1);
        float v111 = get_voxel(xi + 1, yi + 1, zi + 1);

        float tx = std::clamp(x - xi, 0.0f, 1.0f);
        float ty = std::clamp(y - yi, 0.0f, 1.0f);
        float tz = std::clamp(z - zi, 0.0f, 1.0f);

        float v0 = b_lerp(v000, v100, v010, v110, tx, ty);
        float v1 = b_lerp(v001, v101, v011, v111, tx, ty);

        return std::lerp(v0, v1, tz);
    }

    glm::vec3 VoxelArray::get_normal(int x, int y, int z) {
        glm::vec3 n = get_first_derivative(x, y, z);

        if (flip_normals) {
            return -glm::normalize(n);
        }
        else {
            return glm::normalize(n);
        }
    }

    glm::vec3 VoxelArray::get_normal(float u, float v, float w) {
        glm::vec3 n = get_first_derivative(u, v, w);

        if (flip_normals) {
            return -glm::normalize(n);
        }
        else {
            return glm::normalize(n);
        }
    }

    glm::vec3 VoxelArray::get_first_derivative(int x, int y, int z) {
        float dx_p1 = get_voxel(x + 1, y, z);
        float dy_p1 = get_voxel(x, y + 1, z);
        float dz_p1 = get_voxel(x, y, z + 1);

        float dx_m1 = get_voxel(x - 1, y, z);
        float dy_m1 = get_voxel(x, y - 1, z);
        float dz_m1 = get_voxel(x, y, z - 1);

        float dx = (dx_p1 - dx_m1) * 0.5f;
        float dy = (dy_p1 - dy_m1) * 0.5f;
        float dz = (dz_p1 - dz_m1) * 0.5f;

        return glm::vec3(dx, dy, dz);
    }

    glm::vec3 VoxelArray::get_first_derivative(float u, float v, float w) {
        const float h = 0.005f;
        const float hh = h * 0.5f;
        const float ih = 1.0f / h;

        float dx_p1 = get_voxel(u + hh, v, w);
        float dy_p1 = get_voxel(u, v + hh, w);
        float dz_p1 = get_voxel(u, v, w + hh);

        float dx_m1 = get_voxel(u - hh, v, w);
        float dy_m1 = get_voxel(u, v - hh, w);
        float dz_m1 = get_voxel(u, v, w - hh);

        float dx = (dx_p1 - dx_m1) * ih;
        float dy = (dy_p1 - dy_m1) * ih;
        float dz = (dz_p1 - dz_m1) * ih;

        return glm::vec3(dx, dy, dz);
    }

    float VoxelArray::b_lerp(float v00, float v10, float v01, float v11, float tx, float ty) {
        return std::lerp(std::lerp(v00, v10, tx), std::lerp(v01, v11, tx), ty);
    }
}