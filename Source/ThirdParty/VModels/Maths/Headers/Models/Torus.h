#pragma once
#include "../Model.h"

namespace vmodel
{
    class Torus : public Model
    {
    public:
        Torus(double R, double r, glm::ivec3 center, glm::ivec3 area_size) : Model(area_size)
        {
            for (int x = 0; x < area_size.x; x++)
                for (int y = 0; y < area_size.y; y++)
                    for (int z = 0; z < area_size.z; z++)
                        func_result.push_back(torus_func(center.x, center.y, center.z, R, r, x, y, z));

            post_construct();
        }

        double torus_func(double X1, double Y1, double Z1, double R1, double r1, double x, double y, double z)
        {
            return pow(sqrt(pow(x - X1, 2) + pow(y - Y1, 2)) - R1, 2) + pow(z - Z1, 2) - pow(r1, 2);
        }
    };
}