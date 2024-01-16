#include "ShaderLexer.h"
#include <Core/Logging/Assert.h>

namespace utils::shaders
{
	constexpr const char* typeToken = "#type";

	const ShaderSources analyzeShaderProgram(const string& source)
	{
		ShaderSources sources;

		size_t typeTokenLength = std::strlen(typeToken);
		size_t pos = source.find(typeToken, 0); // Start of shader type declaration line
		while (pos != string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos);
			VOXEL_CORE_ASSERT(eol != string::npos, "syntax error");
			size_t begin = pos + typeTokenLength + 1; // Start of shader type name (after "#type " keyword)
			string type = source.substr(begin, eol - begin);
			ShaderStage shaderStage = shaderStageFromString(type);
			VOXEL_CORE_ASSERT(shaderStage, "invalid shader type specified!");

			size_t nextLinePos = source.find_first_not_of("\r\n", eol); // Start of shader code after shader type declaration line
			VOXEL_CORE_ASSERT(nextLinePos != string::npos, "syntax error");
			pos = source.find(typeToken, nextLinePos); // Start of next shader type declaration line

			sources[shaderStage] = (pos == string::npos)
				? source.substr(nextLinePos)
				: source.substr(nextLinePos, pos - nextLinePos);
		}

		if (sources.size() > 0)
			return sources;
		else
			return ShaderSources(0);
	}
}