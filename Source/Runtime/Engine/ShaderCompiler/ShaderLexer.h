#pragma once
#include <Renderer/Shader.h>

namespace utils::shaders
{
	using ShaderSources = std::unordered_map<ShaderStage, string>;
	using ShaderBinaries = std::unordered_map<ShaderStage, std::vector<uint32>>;

	ShaderSources analyzeShaderProgram(const string& source);
}