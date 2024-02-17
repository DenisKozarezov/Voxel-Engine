#pragma once
#include "./Headers/Model.h"

namespace vmodel
{
    void Model::post_construct() {
        normalize_value();
    }

    void Model::normalize_value() {
        double max_value = std::numeric_limits<double>::min();

        int it = 0;
        for (int z = 0; z < area_size.z; z++)
            for (int y = 0; y < area_size.y; y++)
                for (int x = 0; x < area_size.x; x++)
                {
                    if (std::abs(func_result[it]) > max_value)
                        max_value = func_result[it];

                    it++;
                }

        for (int i = 0; i < func_result.size(); i++)
            func_result[i] /= max_value;
    }

    std::vector<glm::ivec3> Model::get_points()
    {
        std::vector<glm::ivec3> p;

        int it = 0;
        for (int x = 0; x < area_size.x; x++)
            for (int y = 0; y < area_size.y; y++)
                for (int z = 0; z < area_size.z; z++) {
                    if (func_result[it] <= 0)
                        p.push_back(glm::ivec3(x, y, z));
                    it++;
                }

        return p;
    }

    std::vector<std::vector<std::vector<GridPoint>>> Model::get_grid()
    {
        auto p = std::vector<std::vector<std::vector<GridPoint>>>();
        p.resize(area_size.x);
        for (int i = 0; i < area_size.x; i++)
        {
            p[i].resize(area_size.y);
            for (int j = 0; j < area_size.y; j++)
                p[i][j].resize(area_size.z);
        }

        int it = 0;
        for (int z = 0; z < area_size.z; z++)
            for (int y = 0; y < area_size.y; y++)
                for (int x = 0; x < area_size.x; x++) {
                    p[x][y][z] = GridPoint(
                        glm::ivec3(x, y, z),
                        -(float)func_result[it]
                    );
                    it++;
                }

        return p;
    }
}