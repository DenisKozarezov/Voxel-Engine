#pragma once
#include <core/Base.h>

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

		INLINE const float getSeconds() const { return m_time * 0.001f; }
		INLINE const float& getMilliseconds() const { return m_time; }
	};
}