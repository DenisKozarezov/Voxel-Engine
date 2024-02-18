#include "./Headers/MarchingCubes.h"

namespace marching
{
    void MarchingCubes::march(float x, float y, float z, float cube[], std::vector<glm::vec3>& vertList, std::vector<int>& indexList) {
        int i, j, vert, idx;
        int flagIndex = 0;
        float offset = 0.0f;

        //Find which vertices are inside of the surface and which are outside
        for (i = 0; i < 8; i++) if (cube[i] <= surface) flagIndex |= 1 << i;

        //Find which edges are intersected by the surface
        int edgeFlags = cube_edge_flags[flagIndex];

        //If the cube is entirely inside or outside of the surface, then there will be no intersections
        if (edgeFlags == 0) return;

        //Find the point of intersection of the surface with each edge
        for (i = 0; i < 12; i++)
        {
            //if there is an intersection on this edge
            if ((edgeFlags & (1 << i)) != 0)
            {
                offset = get_offset(cube[edge_connection[i][0]], cube[edge_connection[i][1]]);

                edge_vertex[i].x = x + (vertex_offset[edge_connection[i][0]][0] + offset * edge_direction[i][0]);
                edge_vertex[i].y = y + (vertex_offset[edge_connection[i][0]][1] + offset * edge_direction[i][1]);
                edge_vertex[i].z = z + (vertex_offset[edge_connection[i][0]][2] + offset * edge_direction[i][2]);
            }
        }

        //Save the triangles that were found. There can be up to five per cube
        for (i = 0; i < 5; i++)
        {
            if (triangle_connection_table[flagIndex][3 * i] < 0) break;

            idx = vertList.size();

            for (j = 0; j < 3; j++)
            {
                vert = triangle_connection_table[flagIndex][3 * i + j];
                indexList.push_back(idx + winding_order[j]);
                vertList.push_back(edge_vertex[vert]);
            }
        }
    }
}