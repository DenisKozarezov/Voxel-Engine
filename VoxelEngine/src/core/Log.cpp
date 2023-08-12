#include "Log.h"
#include <spdlog/sinks/stdout_color_sinks.h>

namespace VoxelEngine
{
	void Log::init()
	{
		spdlog::set_pattern("[%H:%M:%S.%e] [%l] %n: %v%$");
		_coreLogger = spdlog::stdout_color_mt("Voxel Engine");
		_coreLogger->set_level(spdlog::level::trace);

		_clientLogger = spdlog::stdout_color_mt("Editor Application");
		_clientLogger->set_level(spdlog::level::trace);
	}
	constexpr SharedRef<spdlog::logger>& Log::getCoreLogger() { return _coreLogger; }
	constexpr SharedRef<spdlog::logger>& Log::getClientLogger() { return _clientLogger; }
}