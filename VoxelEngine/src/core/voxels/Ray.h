#pragma once
#include <glm/glm.hpp>

class Ray 
{
private:
    constexpr glm::dvec3 inverseRay() noexcept
    {
        if (direction.x == 0.0 || direction.y == 0.0 || direction.z == 0.0)
            return glm::dvec3(0.0f);

        return glm::dvec3(
            1.0 / direction.x,
            1.0 / direction.y,
            1.0 / direction.z);
    }
public:
    glm::dvec3 origin;
    glm::dvec3 direction;
    glm::dvec3 inverse;

    Ray() noexcept = default;
    constexpr Ray(const glm::dvec3& origin, const glm::dvec3& direction) noexcept
    {
        this->origin = origin;
        this->direction = direction;
        inverse = inverseRay();
    }
    constexpr Ray(const Ray& rhs) noexcept : Ray(rhs.origin, rhs.direction) { }
    constexpr Ray(Ray&& rhs) noexcept : Ray(std::move(rhs.origin), std::move(rhs.direction)) { }
    constexpr Ray& operator=(const Ray& rhs)
    {
        origin = rhs.origin;
        direction = rhs.direction;
        inverse = inverseRay();
        return *this;
    }

    ~Ray() noexcept = default;
};