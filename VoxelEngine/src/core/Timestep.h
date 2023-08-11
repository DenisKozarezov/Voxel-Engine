#pragma once

namespace VoxelEngine
{
	class Timestep final
	{
	private:
		float m_time;
	public:
		Timestep(const float& time) noexcept : m_time(time) { }
		~Timestep() noexcept = default;

		constexpr operator float() const & { return m_time; }

		inline constexpr float getSeconds() { return m_time * 0.001f; }
		inline constexpr float& getMilliseconds() & { return m_time; }
	};
}