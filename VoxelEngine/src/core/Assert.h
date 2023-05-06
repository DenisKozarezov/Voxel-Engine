#pragma once

namespace VoxelEngine
{
#ifdef VOXEL_ENABLE_ASSERTS
	#define GROOVY_ASSERT(x, ...) { if (!x) { GROOVY_ERROR("Assertion Failed: {0}", __VA_ARGS__); GROOVY_DEBUGBREAK() } }
	#define GROOVY_CORE_ASSERT(x, ...) { if (!x) { GROOVY_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); GROOVY_DEBUGBREAK() } }
#else
	#define GROOVY_ASSERT(x, ...)
	#define GROOVY_CORE_ASSERT(x, ...)
#endif
}