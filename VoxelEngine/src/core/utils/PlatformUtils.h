#pragma once
#include "../PrimitiveTypes.h"

namespace utils
{
	static class FileDialog
	{
	public:
		static const char* openFile(const char* filter);
		static const char* saveFile(const char* filter);
	};
}