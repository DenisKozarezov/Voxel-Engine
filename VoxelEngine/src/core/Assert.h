#pragma once
#include "Log.h"
#include "PrimitiveTypes.h"

namespace VoxelEngine
{
#define VOXEL_EXPAND_MACRO(x) x
#define VOXEL_STRINGIFY_MACRO(x) #x

#ifdef VOXEL_ENABLE_ASSERTS
	#define VOXEL_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { VOXEL##type##ERROR(msg, __VA_ARGS__); VOXEL_DEBUGBREAK(); } }
	#define VOXEL_INTERNAL_ASSERT_WITH_MSG(type, check, ...) VOXEL_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
	#define VOXEL_INTERNAL_ASSERT_NO_MSG(type, check) VOXEL_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", VOXEL_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)
	
	#define VOXEL_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
	#define VOXEL_INTERNAL_ASSERT_GET_MACRO(...) VOXEL_EXPAND_MACRO( VOXEL_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, VOXEL_INTERNAL_ASSERT_WITH_MSG, VOXEL_INTERNAL_ASSERT_NO_MSG) )
	
	#define VOXEL_ASSERT(...) VOXEL_EXPAND_MACRO( VOXEL_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__) )
	#define VOXEL_CORE_ASSERT(...) VOXEL_EXPAND_MACRO( VOXEL_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__) )
#else
	#define VOXEL_ASSERT(...)
	#define VOXEL_CORE_ASSERT(...)
#endif
}