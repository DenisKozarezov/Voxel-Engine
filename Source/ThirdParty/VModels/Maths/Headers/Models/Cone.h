#pragma once
#include "../Model.h"

namespace vmodel
{
    class Cone : public Model
    {
    public:
        Cone(double a, double b, double c, glm::ivec3 center, Direction dir, glm::ivec3 area_size) : Model(area_size)
        {
            for (int x = 0; x < area_size.x; x++)
                for (int y = 0; y < area_size.y; y++)
                    for (int z = 0; z < area_size.z; z++)
                        func_result.push_back(cone_func(a, b, c, dir, center.x, center.y, center.z, x, y, z));

            post_construct();
        }

        double cone_func(double a, double b, double c, Direction dir, double X1, double Y1, double Z1, double x, double y, double z)
        {
            switch (dir)
            {
            case Direction::OX:
                return pow(x - X1, 2) / (a * a) + pow(y - Y1, 2) / (b * b) - pow(z - Z1, 2) / (c * c);
            case Direction::OY:
                return pow(x - X1, 2) / (a * a) + pow(z - Z1, 2) / (b * b) - pow(y - Y1, 2) / (c * c);
            case Direction::OZ:
                return pow(y - Y1, 2) / (a * a) + pow(z - Z1, 2) / (b * b) - pow(x - X1, 2) / (c * c);
            default:
                return 0;
            }
        }
    };
}