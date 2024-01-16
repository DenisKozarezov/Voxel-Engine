#include "Box.h"

constexpr bool Box::intersect(const Ray& ray, double tMin, double tMax) const
{
    for (int i = 0; i < 3; ++i) 
    {
        double t1 = (m_min[i] - ray.origin[i]) * ray.inverse[i];
        double t2 = (m_max[i] - ray.origin[i]) * ray.inverse[i];

        tMin = std::max(tMin, std::min(t1, t2));
        tMax = std::min(tMax, std::max(t1, t2));
    }
    return tMax > std::max(tMin, 0.0);
}
