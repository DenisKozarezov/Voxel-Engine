#pragma once
#include <cstdint>

namespace VoxelEngine
{
	// ------- unsigned -------
	using ulong = unsigned long;
	using uint64 = uint64_t;
	using uint32 = uint32_t;
	using uint16 = uint16_t;
	using uint8 = uint8_t;
	using byte = unsigned char;

	// -------- signed --------
	using slong = signed long;
	using int64 = int64_t;
	using int16 = int16_t;
	using sbyte = signed char;

	// -------- hybrid -------- 
	using ldouble = long double;

	using string = std::string;

#define BIT(x) (1 << x)
}