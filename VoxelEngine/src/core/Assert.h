#pragma once

namespace VoxelEngine
{
#ifdef VOXEL_ENABLE_ASSERTS
	#define VOXEL_ASSERT(x, ...) { if (!x) { VOXEL_ERROR("Assertion Failed: {0}", __VA_ARGS__); VOXEL_DEBUGBREAK() } }
	#define VOXEL_CORE_ASSERT(x, ...) { if (!x) { VOXEL_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); VOXEL_DEBUGBREAK() } }
#else
	#define VOXEL_ASSERT(x, ...)
	#define VOXEL_CORE_ASSERT(x, ...)
#endif
}