#pragma once

#ifdef _WIN32
	#ifdef _WIN64
		#define VOXEL_PLATFORM_WINDOWS
	#else
		#error "x86 Builds are not supported!"
	#endif
#elif __APPLE__ || __MAC__
	#include <TargetConditionals.h>
#if TARGET_IPHONE_SIMULATOR == 1
	#error "IOS simulator is not supported!"
#elif TARGET_OS_IPHONE == 1
	#define VOXEL_PLATFORM_IOS
	#error "IOS is not supported!"
#elif TARGET_OS_MAC == 1
	#define VOXEL_PLATFORM_MACOS
	#error "MacOS is not supported!"
#else
	#error "Unknown Apple platform!"
	#endif

#elif __ANDROID__
	#define VOXEL_PLATFORM_ANDROID
	#error "Android is not supported!"

#elif __LINUX__ || _DIRECT2DISPLAY
	#define VOXEL_PLATFORM_LINUX
#else
	#error "Unknown platform!"
#endif