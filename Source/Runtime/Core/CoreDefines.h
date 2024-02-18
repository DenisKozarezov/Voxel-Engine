#pragma once
#include <memory>
#include <Core/HAL/Platform/PlatformDetection.h>

#ifdef VOXEL_PLATFORM_WINDOWS
	#ifdef VOXELENGINE_EXPORTS
	/// This macro is used to import/export Voxel API from the DLL library. 
	#define VOXEL_API DLLEXPORT
	#else 
	#define VOXEL_API DLLIMPORT
	#endif
#else
	#error Voxel Engine only supports Windows platform.
#endif

#ifdef VOXEL_DEBUG
	#ifdef VOXEL_PLATFORM_WINDOWS
		#define PLATFORM_BREAK() (__nop(), __debugbreak())
	#elif VOXEL_PLATFORM_LINUX
		#include <signal.h>
		#define PLATFORM_BREAK() raise(SIGTRAP)
	#endif
		#define VOXEL_ENABLE_ASSERTS
	#else
	#define PLATFORM_BREAK()
#endif

#define INLINE inline
#define FORCE_INLINE __forceinline
#define NODISCARD [[nodiscard]]

#define BIND_CALLBACK(cb) [this](auto&&... args) -> decltype(auto) { return this->cb(std::forward<decltype(args)>(args)...); }
#define BIND_MEMBER_CALLBACK(target, cb) [this](auto&&... args) -> decltype(auto) { return (*target).cb(std::forward<decltype(args)>(args)...); }

// DLL export and import definitions
#define DLLEXPORT __declspec(dllexport)
#define DLLIMPORT __declspec(dllimport)

enum { INDEX_NONE = -1 };

template<typename T>
using TUniquePtr = std::unique_ptr<T>;
template<typename T, typename ...Args>
constexpr TUniquePtr<T> MakeUnique(Args&& ... args)
{
	return std::make_unique<T>(std::forward<Args>(args)...);
}

template<typename T>
using TSharedPtr = std::shared_ptr<T>;
template<typename T, typename ...Args>
constexpr TSharedPtr<T> MakeShared(Args&& ... args)
{
	return std::make_shared<T>(std::forward<Args>(args)...);
}

template<typename T>
using TWeakPtr = std::weak_ptr<T>;
template<typename T, typename ...Args>
constexpr TWeakPtr<T> MakeWeak(Args&& ... args)
{
	return std::weak_ptr<T>(std::forward<Args>(args)...);
}