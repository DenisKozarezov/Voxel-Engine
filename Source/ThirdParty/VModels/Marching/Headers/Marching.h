#pragma once
#include <vector>
#include <glm/vec3.hpp>

namespace marching
{
    class Marching {
    private:
        float cube[8];
        virtual void update_winding_order();
    protected:
        int winding_order[3];
        static constexpr int vertex_offset[8][3] = {
            {0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0},
            {0, 0, 1}, {1, 0, 1}, {1, 1, 1}, {0, 1, 1}
        };
        virtual void march(float x, float y, float z, float cube[], std::vector<glm::vec3>& vertList, std::vector<int>& indexList) = 0;
        virtual float get_offset(float v1, float v2);
    public:
        float surface;

        Marching(float surface) : surface(surface), cube(), winding_order{0, 1, 2} {}
        virtual ~Marching() = default;

        virtual void generate(std::vector<std::vector<std::vector<float>>> voxels, std::vector<glm::vec3>& verts, std::vector<int>& indices);
        virtual void generate(std::vector<std::vector<std::vector<float>>> voxels, int width, int height, int depth, std::vector<glm::vec3>& verts, std::vector<int>& indices);
    };
}