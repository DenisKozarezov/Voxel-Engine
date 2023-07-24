#pragma once
#include <memory>
#include "PlatformDetection.h"

#ifdef VOXEL_PLATFORM_WINDOWS
	#ifdef VOXELENGINE_EXPORTS
	/// This macro is used to import/export Voxel API from the DLL library. 
	#define VOXEL_API __declspec(dllexport)
	#else 
	#define VOXEL_API __declspec(dllimport)
	#endif
#else
	#error Voxel Engine only supports Windows platform.
#endif

#ifdef VOXEL_DEBUG
	#ifdef VOXEL_PLATFORM_WINDOWS
		#define VOXEL_DEBUGBREAK() __debugbreak();
	#elif VOXEL_PLATFORM_LINUX
		#include <signal.h>
		#define VOXEL_DEBUGBREAK() raise(SIGTRAP);
	#endif
		#define VOXEL_ENABLE_ASSERTS
	#else
	#define VOXEL_DEBUGBREAK()
#endif

#define BIND_CALLBACK(cb) [this](auto&&... args) -> decltype(auto) { return this->cb(std::forward<decltype(args)>(args)...); }

namespace VoxelEngine
{
	template<typename T>
	using UniqueRef = std::unique_ptr<T>;
	template<typename T, typename ...Args>
	constexpr UniqueRef<T> MakeUnique(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using SharedRef = std::shared_ptr<T>;
	template<typename T, typename ...Args>
	constexpr SharedRef<T> MakeShared(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}