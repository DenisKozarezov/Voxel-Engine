#pragma once
#include "Ray.h"
#include <Core/Logging/Assert.h>

class Box 
{
private:
	glm::vec3 m_min, m_max;
public:
	constexpr Box() : Box(glm::vec3(0.0f), glm::vec3(1.0f)) { }
	constexpr Box(const glm::vec3& min, const glm::vec3& max)
		: m_min(min), m_max(max)
	{
		EDITOR_ASSERT(min.x < max.x && min.y < max.y, "box's min must be less than max!");
	}
	
	FORCE_INLINE const glm::vec3& min() const { return m_min; }
	FORCE_INLINE const glm::vec3& max() const { return m_max; }
	FORCE_INLINE glm::vec3 center() const { return (m_max - m_min) * 0.5f + m_min; }

	constexpr bool intersect(const Ray& ray, double tMin = -INFINITY, double tMax = INFINITY) const;
	constexpr bool inside(const glm::vec3& point) const
	{
		return
			(point.x >= m_min.x && point.x <= m_max.x) &&
			(point.y >= m_min.y && point.y <= m_max.y) &&
			(point.z >= m_min.z && point.z <= m_max.z);
	}
	constexpr bool inside(const glm::vec3* points, const int& size) const
	{
		for (int i = 0; i < size; i++) 
		{
			if (!inside(points[i])) 
				return false;
		}
		return true;
	}
};
