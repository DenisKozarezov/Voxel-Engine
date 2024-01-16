#pragma once
#include <Core/Internal/utils/EnumUtils.h>

namespace utils::shaders
{
	using ShaderSources = std::unordered_map<ShaderStage, string>;
	using ShaderBinaries = std::unordered_map<ShaderStage, std::vector<uint32>>;

	const ShaderSources analyzeShaderProgram(const string& source);
}