#pragma once
#include <core/utils/EnumUtils.h>

namespace utils::lexer
{
	using ShaderSources = std::unordered_map<ShaderStage, string>;
	using ShaderBinaries = std::unordered_map<ShaderStage, std::vector<uint32>>;

	const ShaderSources analyzeShaderProgram(const string& source);
}