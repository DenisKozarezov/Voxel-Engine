#pragma once
#include "../PrimitiveTypes.h"

namespace utils
{
	static class FileDialog
	{
	public:
		static const string openFile(const char* filter);
		static const string saveFile(const char* filter);
	};
}