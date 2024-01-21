#pragma once
#include <Core/CoreDefines.h>

namespace VoxelEngine
{
	class Timestep final
	{
	private:
		double m_time;
	public:
		Timestep(const double& time) noexcept : m_time(time) { }
		~Timestep() noexcept = default;

		constexpr operator double&() { return m_time; }
		constexpr operator double() const& { return m_time; }

		FORCE_INLINE double getSeconds() const { return m_time * 0.001; }
		FORCE_INLINE const double& getMilliseconds() const { return m_time; }
	};
}