#include "ShaderLexer.h"
#include <Core/Logging/Assert.h>

namespace utils::shaders
{
	constexpr const char* typeToken = "#type";

	ShaderSources analyzeShaderProgram(const string& source)
	{
		ShaderSources sources;

		const size_t typeTokenLength = std::strlen(typeToken);
		size_t pos = source.find(typeToken, 0); // Start of shader type declaration line
		while (pos != string::npos)
		{
			const size_t eol = source.find_first_of("\r\n", pos);
			RUNTIME_ASSERT(eol != string::npos, "syntax error");
			const size_t begin = pos + typeTokenLength + 1; // Start of shader type name (after "#type " keyword)
			string type = source.substr(begin, eol - begin);
			ShaderStage shaderStage = shaderStageFromString(type);
			RUNTIME_ASSERT(shaderStage != ShaderStage::None, "invalid shader type specified!");

			const size_t nextLinePos = source.find_first_not_of("\r\n", eol); // Start of shader code after shader type declaration line
			RUNTIME_ASSERT(nextLinePos != string::npos, "syntax error");
			pos = source.find(typeToken, nextLinePos); // Start of next shader type declaration line

			sources[shaderStage] = (pos == string::npos)
				? source.substr(nextLinePos)
				: source.substr(nextLinePos, pos - nextLinePos);
		}

		if (!sources.empty())
			return sources;
		else
			return ShaderSources(0);
	}
}