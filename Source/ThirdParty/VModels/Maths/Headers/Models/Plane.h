#pragma once
#include "../Model.h"

namespace vmodel
{
    class Plane : public Model
    {
    public:
        Plane(glm::ivec3 p1, glm::ivec3 p2, glm::ivec3 p3, glm::ivec3 area_size) : Model(area_size)
        {
            for (int x = 0; x < area_size.x; x++)
                for (int y = 0; y < area_size.y; y++)
                    for (int z = 0; z < area_size.z; z++)
                        func_result.push_back(plane_func(p1, p2, p3, x, y, z));

            post_construct();
        }
        
        double plane_func(glm::ivec3 p1, glm::ivec3 p2, glm::ivec3 p3, double x, double y, double z)
        {
            return (x - p1.x) * ((p2.y - p1.y) * (p3.z - p1.z) - (p2.z - p1.z) * (p3.y - p1.y))
                - (y - p1.y) * ((p2.x - p1.x) * (p3.z - p1.z) - (p2.z - p1.z) * (p3.x - p1.x))
                + (z - p1.z) * ((p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x));
        }
    };
}