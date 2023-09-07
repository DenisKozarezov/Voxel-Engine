#pragma once
#include <core/Assert.h>
#include "Ray.h"

class Box 
{
private:
	glm::vec3 m_min, m_max;
public:
	Box() noexcept = default;
	constexpr Box(const glm::vec3& min, const glm::vec3& max) 
	{
		VOXEL_CORE_ASSERT(min.x < max.x && min.y < max.y, "box's min must be less than max!");

		this->m_min = min;
		this->m_max = max;
	}
	
	INLINE const glm::vec3& min() const { return m_min; }
	INLINE const glm::vec3& max() const { return m_max; }
	INLINE const glm::vec3 center() const { return (m_max - m_min) * 0.5f + m_min; }

	constexpr bool intersect(const Ray& ray, double tMin = -INFINITY, double tMax = INFINITY) const;
	constexpr bool inside(const glm::vec3& point)
	{
		return
			(point.x >= m_min.x && point.x <= m_max.x) &&
			(point.y >= m_min.y && point.y <= m_max.y) &&
			(point.z >= m_min.z && point.z <= m_max.z);
	}
	constexpr bool inside(const glm::vec3* points, const int& size) 
	{
		for (int i = 0; i < size; i++) 
		{
			if (!inside(points[i])) 
				return false;
		}
		return true;
	}
};