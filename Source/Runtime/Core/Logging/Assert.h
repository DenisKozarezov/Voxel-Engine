#pragma once
#include "Log.h"
#include "../CoreTypes.h"

namespace VoxelEngine
{
#define ASSERT_EXPAND_MACRO(x) x
#define ASSERT_STRINGIFY_MACRO(x) #x

#ifdef VOXEL_ENABLE_ASSERTS
	#define ASSERT_INTERNAL_IMPL(type, check, msg, ...) { if(!(check)) { ##type##ERROR(msg, __VA_ARGS__); PLATFORM_BREAK(); } }
	#define ASSERT_INTERNAL_WITH_MSG(type, check, ...) ASSERT_INTERNAL_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
	#define ASSERT_INTERNAL_NO_MSG(type, check) ASSERT_INTERNAL_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", ASSERT_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)
	
	#define ASSERT_INTERNAL_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
	#define ASSERT_INTERNAL_GET_MACRO(...) ASSERT_EXPAND_MACRO( ASSERT_INTERNAL_GET_MACRO_NAME(__VA_ARGS__, ASSERT_INTERNAL_WITH_MSG, ASSERT_INTERNAL_NO_MSG) )
	
	#define EDITOR_ASSERT(...) ASSERT_EXPAND_MACRO( ASSERT_INTERNAL_GET_MACRO(__VA_ARGS__)(EDITOR_, __VA_ARGS__) )
	#define RUNTIME_ASSERT(...) ASSERT_EXPAND_MACRO( ASSERT_INTERNAL_GET_MACRO(__VA_ARGS__)(RUNTIME_, __VA_ARGS__) )
#else
	#define EDITOR_ASSERT(...)
	#define RUNTIME_ASSERT(...)
#endif
}