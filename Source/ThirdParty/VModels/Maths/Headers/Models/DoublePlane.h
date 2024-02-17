#pragma once
#include "../Model.h"

namespace vmodel
{
    class DoublePlane : public Model
    {
    public:
        DoublePlane(double b, glm::ivec3 center, Direction dir, glm::ivec3 area_size) : Model(area_size)
        {
            for (int x = 0; x < area_size.x; x++)
                for (int y = 0; y < area_size.y; y++)
                    for (int z = 0; z < area_size.z; z++)
                        func_result.push_back(double_plane_func(b, dir, center.x, center.y, center.z, x, y, z));

            post_construct();
        }

        double double_plane_func(double b, Direction dir, double X1, double Y1, double Z1, double x, double y, double z)
        {
            switch (dir)
            {
            case Direction::OX:
                return pow(z - Z1, 2) - b * b;
            case Direction::OY:
                return pow(y - Y1, 2) - b * b;
            case Direction::OZ:
                return pow(x - X1, 2) - b * b;
            default:
                return 0;
            }
        }
    };
}