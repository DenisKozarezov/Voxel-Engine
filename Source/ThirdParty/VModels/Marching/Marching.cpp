#include "./Headers/Marching.h"

namespace marching
{
    void Marching::generate(std::vector<std::vector<std::vector<float>>> voxels, std::vector<glm::vec3>& verts, std::vector<int>& indices)
    {
        int width = voxels.size();
        int height = voxels[0].size();
        int depth = voxels[0][0].size();

        update_winding_order();

        int x, y, z, i;
        int ix = 0, iy = 0, iz = 0;
        for (x = 0; x < width - 1; x++)
        {
            for (y = 0; y < height - 1; y++)
            {
                for (z = 0; z < depth - 1; z++)
                {
                    for (i = 0; i < 8; i++)
                    {
                        ix = x + vertex_offset[i][0];
                        iy = y + vertex_offset[i][1];
                        iz = z + vertex_offset[i][2];

                        cube[i] = voxels[ix][iy][iz];
                    }

                    march(x, y, z, cube, verts, indices);
                }
            }
        }
    }

    void Marching::generate(std::vector<std::vector<std::vector<float>>> voxels, int width, int height, int depth, std::vector<glm::vec3>& verts, std::vector<int>& indices)
    {
        update_winding_order();

        int x, y, z, i;
        int ix = 0, iy = 0, iz = 0;
        for (x = 0; x < width - 1; x++)
        {
            for (y = 0; y < height - 1; y++)
            {
                for (z = 0; z < depth - 1; z++)
                {
                    for (i = 0; i < 8; i++)
                    {
                        ix = x + vertex_offset[i][0];
                        iy = y + vertex_offset[i][1];
                        iz = z + vertex_offset[i][2];

                        cube[i] = voxels[ix][iy][iz];
                    }

                    march(x, y, z, cube, verts, indices);
                }
            }
        }
    }

    void Marching::update_winding_order() {
        if (surface > 0.0f) {
            winding_order[0] = 2;
            winding_order[1] = 1;
            winding_order[2] = 0;
        }
        else {
            winding_order[0] = 0;
            winding_order[1] = 1;
            winding_order[2] = 2;
        }
    }

    float Marching::get_offset(float v1, float v2) {
        float delta = v2 - v1;
        return (delta == 0.0f) ? surface : (surface - v1) / delta;
    }
}
