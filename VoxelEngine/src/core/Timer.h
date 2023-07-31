#pragma once
#include <chrono>

namespace VoxelEngine
{
	class Timer final
	{
	private:
		std::chrono::time_point<std::chrono::steady_clock> _start;
	public:
		Timer()
		{
			reset();
		}

		template<typename TCast>
		requires std::is_floating_point_v<TCast>
		constexpr float elapsedTimeInMilliseconds() const
		{
			return std::chrono::duration<TCast, std::milli>(std::chrono::high_resolution_clock::now() - _start).count();
		}
		void reset()
		{
			_start = std::chrono::high_resolution_clock::now();
		}
	};
}