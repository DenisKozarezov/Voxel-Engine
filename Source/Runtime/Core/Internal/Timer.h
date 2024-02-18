#pragma once
#include <chrono>
#include <Core/CoreDefines.h>

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

		template<typename TCast = double>
		requires std::is_floating_point_v<TCast>
		FORCE_INLINE TCast elapsedTimeInMilliseconds() const
		{
			return std::chrono::duration<TCast, std::milli>(std::chrono::high_resolution_clock::now() - m_start).count();
		}

		template<typename TCast = double>
		requires std::is_floating_point_v<TCast>
		FORCE_INLINE TCast elapsedTimeInMicroseconds() const
		{
			return std::chrono::duration<TCast, std::micro>(std::chrono::high_resolution_clock::now() - m_start).count();
		}

		template<typename TCast = double>
		requires std::is_floating_point_v<TCast>
		FORCE_INLINE TCast elapsedTimeInNanoseconds() const
		{
			return std::chrono::duration<TCast, std::nano>(std::chrono::high_resolution_clock::now() - m_start).count();
		}

		FORCE_INLINE void reset()
		{
			m_start = std::chrono::high_resolution_clock::now();
		}
	};
}