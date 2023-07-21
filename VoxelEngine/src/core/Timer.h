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
		const float elapsedInMilliseconds() const
		{
			return std::chrono::duration<TCast, std::milli>(std::chrono::high_resolution_clock::now() - _start).count() * 0.001f;
		}
		void reset()
		{
			_start = std::chrono::high_resolution_clock::now();
		}
	};
}