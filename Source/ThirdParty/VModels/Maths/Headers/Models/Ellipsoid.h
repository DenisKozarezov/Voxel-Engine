#pragma once
#include "../Model.h"

namespace vmodel
{
    class Ellipsoid : public Model
    {
    public:
        Ellipsoid(double a, double b, double c, glm::ivec3 center, glm::ivec3 area_size) : Model(area_size)
        {
            for (int x = 0; x < area_size.x; x++)
                for (int y = 0; y < area_size.y; y++)
                    for (int z = 0; z < area_size.z; z++)
                        func_result.push_back(ellipsoid_func(a, b, c, center.x, center.y, center.z, x, y, z));
        
            post_construct();
        }

        double ellipsoid_func(double a, double b, double c, double X1, double Y1, double Z1, double x, double y, double z)
        {
            return pow(x - X1, 2) / (a * a) + pow(y - Y1, 2) / (b * b) + pow(z - Z1, 2) / (c * c) - 1;
        }
    };
}