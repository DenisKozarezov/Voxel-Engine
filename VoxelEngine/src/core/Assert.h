#pragma once
#include "Log.h"

namespace VoxelEngine
{
#ifdef VOXEL_ENABLE_ASSERTS
	#define VOXEL_ASSERT(x, ...) { if (!x) { VOXEL_CRITICAL("Assertion Failed: {0}", __VA_ARGS__); VOXEL_DEBUGBREAK() } }
	#define VOXEL_CORE_ASSERT(x, ...) { if (!x) { VOXEL_CORE_CRITICAL("Assertion Failed: {0}", __VA_ARGS__); VOXEL_DEBUGBREAK() } }
#else
	#define VOXEL_ASSERT(...)
	#define VOXEL_CORE_ASSERT(...)
#endif
}