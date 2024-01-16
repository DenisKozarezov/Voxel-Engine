#pragma once
#include <vector>
#include <Renderer/Shader.h>

namespace utils::shaders
{
    class ShaderCompiler
    {
    public:
        ShaderCompiler() = default;
        ~ShaderCompiler() = default;
        
        std::vector<uint32> compileToSpirv(const string& shaderProgram, const char* filepath, const ShaderStage& stage) const;
    };
}