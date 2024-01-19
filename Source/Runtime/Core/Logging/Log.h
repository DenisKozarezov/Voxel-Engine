#pragma once
#include <Core/CoreDefines.h>
#include <spdlog/spdlog.h>

constexpr std::string logLevelString(const spdlog::level::level_enum& level)
{
	switch (level)
	{
#define STR(x) case spdlog::level::##x: return #x
		STR(info);
		STR(warn);
		STR(critical);
		STR(err);
#undef STR
	default: return "info";
	}
}

namespace VoxelEngine
{
	class VOXEL_API Log
	{
	private:
		INLINE static TSharedPtr<spdlog::logger> _coreLogger;
		INLINE static TSharedPtr<spdlog::logger> _clientLogger;
	public:
		static constexpr const char* s_timePattern = "%H:%M:%S";
		static constexpr const char* s_logPattern = "[%H:%M:%S.%e] [%l] %n: %v%$";

		static void init(const char* coreProject, const char* editorProject);
		FORCE_INLINE static const TSharedPtr<spdlog::logger>& getCoreLogger() { return _coreLogger; }
		FORCE_INLINE static const TSharedPtr<spdlog::logger>& getClientLogger() { return _clientLogger; }
	};

#define FORMATTED_TIME string("{0:") + VoxelEngine::Log::s_timePattern + string("}")

#define RUNTIME_TRACE(...)		::VoxelEngine::Log::getCoreLogger()->trace(__VA_ARGS__);
#define RUNTIME_INFO(...)		::VoxelEngine::Log::getCoreLogger()->info(__VA_ARGS__);
#define RUNTIME_WARN(...)		::VoxelEngine::Log::getCoreLogger()->warn(__VA_ARGS__);
#define RUNTIME_ERROR(...)		::VoxelEngine::Log::getCoreLogger()->error(__VA_ARGS__);
#define RUNTIME_CRITICAL(...)	::VoxelEngine::Log::getCoreLogger()->critical(__VA_ARGS__);

#define EDITOR_TRACE(...)			::VoxelEngine::Log::getClientLogger()->trace(__VA_ARGS__);
#define EDITOR_INFO(...)			::VoxelEngine::Log::getClientLogger()->info(__VA_ARGS__);
#define EDITOR_WARN(...)			::VoxelEngine::Log::getClientLogger()->warn(__VA_ARGS__);
#define EDITOR_ERROR(...)			::VoxelEngine::Log::getClientLogger()->error(__VA_ARGS__);
#define EDITOR_CRITICAL(...)		::VoxelEngine::Log::getClientLogger()->critical(__VA_ARGS__);
}
