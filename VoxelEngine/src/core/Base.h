#pragma once
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