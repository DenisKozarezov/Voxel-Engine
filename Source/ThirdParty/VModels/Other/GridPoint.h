#pragma once
#include <glm/vec3.hpp>

struct GridPoint {
public:
    glm::ivec3 position;
    float value;

    GridPoint() = default;
    GridPoint(glm::ivec3 pos, float val) : position(pos), value(val) {}
    ~GridPoint() = default;
};