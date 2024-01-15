#pragma once
#include "Base.h"
#include <spdlog/spdlog.h>

namespace VoxelEngine
{
	class VOXEL_API Log
	{
	private:
		INLINE static SharedRef<spdlog::logger> _coreLogger;
		INLINE static SharedRef<spdlog::logger> _clientLogger;
	public:
		static constexpr const char* s_timePattern = "%H:%M:%S";
		static constexpr const char* s_logPattern = "[%H:%M:%S.%e] [%l] %n: %v%$";

		static void init(const char* coreProject, const char* editorProject);
		FORCE_INLINE static const SharedRef<spdlog::logger>& getCoreLogger() { return _coreLogger; }
		FORCE_INLINE static const SharedRef<spdlog::logger>& getClientLogger() { return _clientLogger; }
	};

#define FORMATTED_TIME string("{0:") + VoxelEngine::Log::s_timePattern + string("}")

#define VOXEL_CORE_TRACE(...)		::VoxelEngine::Log::getCoreLogger()->trace(__VA_ARGS__);
#define VOXEL_CORE_INFO(...)		::VoxelEngine::Log::getCoreLogger()->info(__VA_ARGS__);
#define VOXEL_CORE_WARN(...)		::VoxelEngine::Log::getCoreLogger()->warn(__VA_ARGS__);
#define VOXEL_CORE_ERROR(...)		::VoxelEngine::Log::getCoreLogger()->error(__VA_ARGS__);
#define VOXEL_CORE_CRITICAL(...)	::VoxelEngine::Log::getCoreLogger()->critical(__VA_ARGS__);

#define VOXEL_TRACE(...)			::VoxelEngine::Log::getClientLogger()->trace(__VA_ARGS__);
#define VOXEL_INFO(...)				::VoxelEngine::Log::getClientLogger()->info(__VA_ARGS__);
#define VOXEL_WARN(...)				::VoxelEngine::Log::getClientLogger()->warn(__VA_ARGS__);
#define VOXEL_ERROR(...)			::VoxelEngine::Log::getClientLogger()->error(__VA_ARGS__);
#define VOXEL_CRITICAL(...)			::VoxelEngine::Log::getClientLogger()->critical(__VA_ARGS__);
}
