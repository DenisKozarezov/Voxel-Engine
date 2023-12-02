#pragma once
#include "../PrimitiveTypes.h"

namespace utils
{
	static class FileDialog
	{
	public:
		static string openFile(const char* filter);
		static string saveFile(const char* filter);
	};
}