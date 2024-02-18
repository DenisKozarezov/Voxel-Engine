#pragma once
#include <Core/CoreTypes.h>

namespace utils
{
	class FileDialog
	{
	public:
		static string openFile(const char* filter);
		static string saveFile(const char* filter);
	};
}