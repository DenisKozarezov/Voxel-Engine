#include "Log.h"
#include <spdlog/sinks/stdout_color_sinks.h>

namespace VoxelEngine
{
	void Log::init(const char* coreProject, const char* editorProject)
	{
		spdlog::set_pattern(s_logPattern);
		_coreLogger = spdlog::stdout_color_mt(coreProject);
		_coreLogger->set_level(spdlog::level::trace);

		_clientLogger = spdlog::stdout_color_mt(editorProject);
		_clientLogger->set_level(spdlog::level::trace);
	}
	INLINE const SharedRef<spdlog::logger>& Log::getCoreLogger() { return _coreLogger; }
	INLINE const SharedRef<spdlog::logger>& Log::getClientLogger() { return _clientLogger; }
}