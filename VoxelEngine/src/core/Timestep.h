#pragma once

namespace VoxelEngine
{
	class Timestep final
	{
	private:
		float _time;
	public:
		Timestep(const float& time) noexcept : _time(time) { }
		~Timestep() noexcept = default;

		constexpr operator float() const & { return _time; }

		inline constexpr float getSeconds() { return _time * 0.001f; }
		inline constexpr float& getMilliseconds() & { return _time; }
	};
}