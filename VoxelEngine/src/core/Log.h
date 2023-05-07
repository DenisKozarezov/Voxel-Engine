#pragma once
#include <spdlog/spdlog.h>
#include "Base.h"

namespace VoxelEngine
{
	static class VOXEL_API Log
	{
	private:
		inline static SharedRef<spdlog::logger> _coreLogger;
		inline static SharedRef<spdlog::logger> _clientLogger;
	public:
		static void init();
		inline static SharedRef<spdlog::logger>& getCoreLogger();
		inline static SharedRef<spdlog::logger>& getClientLogger();
	};

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