#pragma once

namespace VoxelEngine
{
	class Timestep final
	{
	private:
		float _time;
	public:
		Timestep(const float& time) : _time(time) { }
		~Timestep() = default;

		operator float() const { return _time; }

		inline const float& getSeconds() const & { return _time; }
		inline const float& getMilliseconds() const { return _time * 1000.0f; }
	};
}