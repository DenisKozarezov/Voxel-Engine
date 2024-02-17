#include "./Headers/Mesh.h"
#include "./Headers/MarchingCubes.h"
#include "./Headers/VoxelArray.h"

namespace marching
{
    MeshData Mesh::build_mesh_data(std::vector<std::vector<std::vector<GridPoint>>> grid_points, glm::ivec3 grid_size, float surface, bool smooth_normals)
    {
        Marching* marching = new MarchingCubes();
        marching->surface = surface;

        int width = grid_size.x;
        int height = grid_size.y;
        int depth = grid_size.z;

        VoxelArray vox_array = VoxelArray(width, height, depth);

        for (int x = 0; x < width; x++)
            for (int y = 0; y < height; y++)
                for (int z = 0; z < depth; z++)
                    vox_array.voxels[x][y][z] = grid_points[x][y][z].value;

        std::vector<glm::vec3> verts;
        std::vector<glm::vec3> normals;
        std::vector<int> indices;

        marching->generate(vox_array.voxels, verts, indices);

        if (smooth_normals) {
            for (int i = 0; i < verts.size(); i++) {
                glm::vec3 p = verts[i];

                float u = p.x / (width - 1.0f);
                float v = p.y / (height - 1.0f);
                float w = p.z / (depth - 1.0f);

                glm::vec3 n = vox_array.get_normal(u, v, w);

                normals.push_back(n);
            }
        }

        delete marching;
        return MeshData(verts, normals, indices);
    }
}