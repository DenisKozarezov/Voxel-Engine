#pragma once
#include "../Model.h"

namespace vmodel
{
    class Sphere : public Model
    {
    public:
        Sphere(double radius, glm::ivec3 center, glm::ivec3 area_size) : Model(area_size)
        {
            for (int x = 0; x < area_size.x; x++)
                for (int y = 0; y < area_size.y; y++)
                    for (int z = 0; z < area_size.z; z++)
                        func_result.push_back(sphere_func(center.x, center.y, center.z, radius, x, y, z));

            post_construct();
        }

        double sphere_func(double X1, double Y1, double Z1, double R1, double x, double y, double z)
        {
            return pow(x - X1, 2) + pow(y - Y1, 2) + pow(z - Z1, 2) - pow(R1, 2);
        }
    };
}