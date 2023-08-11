#pragma once
#include <chrono>

namespace VoxelEngine
{
	class Timer final
	{
	private:
		std::chrono::time_point<std::chrono::steady_clock> m_start;
	public:
		Timer()
		{
			reset();
		}

		template<typename TCast>
		requires std::is_floating_point_v<TCast>
		constexpr float elapsedTimeInMilliseconds() const
		{
			return std::chrono::duration<TCast, std::milli>(std::chrono::high_resolution_clock::now() - m_start).count();
		}
		void reset()
		{
			m_start = std::chrono::high_resolution_clock::now();
		}
	};
}