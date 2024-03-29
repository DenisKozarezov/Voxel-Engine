﻿#include "ShaderCompiler.h"
#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>
#include <Core/HAL/AssetsManager/Paths.h>
#include <Core/Logging/Assert.h>
#include <array>

namespace utils::shaders
{
    class ShaderIncluder : public shaderc::CompileOptions::IncluderInterface
    {
    private:
        string m_program;
    public:
        ShaderIncluder(const string& program) : m_program(program) { }
        
        shaderc_include_result* GetInclude(const char* requestedSource, shaderc_include_type type, const char* requestingSource, size_t includeDepth) override
        {
            string msg = string(requestingSource);
            msg += std::to_string(type);
            msg += static_cast<char>(includeDepth);

            const string name = Paths::shaderWorkingDir() + "include/" + string(requestedSource);

            auto container = new std::array<string, 2>;
            (*container)[0] = name;
            (*container)[1] = m_program;

            auto data = new shaderc_include_result;

            data->user_data = container;

            data->source_name = (*container)[0].data();
            data->source_name_length = (*container)[0].size();

            data->content = (*container)[1].data();
            data->content_length = (*container)[1].size();

            return data;
        }
        void ReleaseInclude(shaderc_include_result* data) override
        {
            delete static_cast<std::array<std::string, 2>*>(data->user_data);
            delete data;
        }
    };

    constexpr shaderc_shader_kind shaderStageToGlslKind(const ShaderStage& stage)
    {
        switch (stage)
        {
        case ShaderStage::Vertex:	return shaderc_glsl_vertex_shader;
        case ShaderStage::Fragment:	return shaderc_glsl_fragment_shader;
        case ShaderStage::Geometry:	return shaderc_glsl_geometry_shader;
        case ShaderStage::Compute:	return shaderc_glsl_compute_shader;
        }
        RUNTIME_ASSERT(false, "unknown shader stage!");
        return (shaderc_shader_kind)0;
    }
    
    std::vector<uint32> ShaderCompiler::compileToSpirv(const string& shaderProgram, const char* filepath,
        const ShaderStage& stage) const
    {
        const shaderc::Compiler compiler;
        shaderc::CompileOptions options;
        options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
        options.SetIncluder(std::make_unique<ShaderIncluder>(shaderProgram));
        const bool optimize = true;
        if (optimize)
            options.SetOptimizationLevel(shaderc_optimization_level_performance);

        shaderc_shader_kind kind = shaderStageToGlslKind(stage);
        shaderc::PreprocessedSourceCompilationResult preprocess = compiler.PreprocessGlsl(shaderProgram, kind, filepath, options);
        RUNTIME_ASSERT(preprocess.GetCompilationStatus() == shaderc_compilation_status_success, preprocess.GetErrorMessage());

        shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(shaderProgram, kind, filepath, options);
        RUNTIME_ASSERT(module.GetCompilationStatus() == shaderc_compilation_status_success, module.GetErrorMessage());
        
        return std::vector<uint32>(module.cbegin(), module.cend());
    }
}
