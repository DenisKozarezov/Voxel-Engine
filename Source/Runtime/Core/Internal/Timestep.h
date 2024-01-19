#pragma once
#include <Core/CoreDefines.h>

namespace VoxelEngine
{
	class Timestep final
	{
	private:
		float m_time;
	public:
		Timestep(const float& time) noexcept : m_time(time) { }
		~Timestep() noexcept = default;

		constexpr operator float&() { return m_time; }
		constexpr operator float() const& { return m_time; }

		FORCE_INLINE float getSeconds() const { return m_time * 0.001f; }
		FORCE_INLINE const float& getMilliseconds() const { return m_time; }
	};
}