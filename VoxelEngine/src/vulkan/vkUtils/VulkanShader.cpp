#include "VulkanShader.h"
#include "VulkanValidation.h"
#include "../vkInit/VulkanInitializers.h"

#include <core/Timer.h>
#include <core/utils/EnumUtils.h>
#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>
#include <assets_management/AssetsProvider.h>

namespace vkUtils
{
	constexpr shaderc_shader_kind shaderStageToShaderStage(const ShaderStage& stage)
	{
		switch (stage)
		{
		case Vertex:	return shaderc_glsl_vertex_shader;
		case Fragment:	return shaderc_glsl_fragment_shader;
		case Geometry:	return shaderc_glsl_geometry_shader;
		case Compute:	return shaderc_glsl_compute_shader;
		}
		VOXEL_CORE_ASSERT(false, "unknown shader stage!");
		return (shaderc_shader_kind)0;
	}
	constexpr const char* getCacheDirectory()
	{
		return CACHE_PATH("shaders/SPIR-V");
	}

	constexpr const char* VulkanShader::shaderStageCachedVulkanFileExtension(const ShaderStage& stage)
	{
		switch (stage)
		{
		case Vertex:    return ".cached_vulkan.vert";
		case Fragment:  return ".cached_vulkan.frag";
		case Geometry:  return ".cached_vulkan.geom";
		case Compute:	return ".cached_vulkan.compute";
		default:
			VOXEL_CORE_ASSERT(false, "unknown shader stage!");
			return nullptr;
		}
	}
	const std::vector<uint32> VulkanShader::compileShaderToSpirv(const std::string& shaderProgram, const char* filepath, const ShaderStage& stage)
	{
		VoxelEngine::Timer timer;

		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
		const bool optimize = true;
		if (optimize)
			options.SetOptimizationLevel(shaderc_optimization_level_performance);

		shaderc_shader_kind kind = shaderStageToShaderStage(stage);
		shaderc::PreprocessedSourceCompilationResult preprocess = compiler.PreprocessGlsl(shaderProgram, kind, filepath, options);
		VOXEL_CORE_ASSERT(preprocess.GetCompilationStatus() == shaderc_compilation_status_success, preprocess.GetErrorMessage());

		shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(shaderProgram, kind, filepath, options);
		VOXEL_CORE_ASSERT(module.GetCompilationStatus() == shaderc_compilation_status_success, module.GetErrorMessage());

		VOXEL_CORE_WARN("Shader '{0}' compilation time: {1} ms.", filepath, timer.elapsedTimeInMilliseconds<double>());

		return std::vector<uint32>(module.cbegin(), module.cend());
	}
	const ShaderSources& VulkanShader::preProcess(const std::string& source)
	{
		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = source.find(typeToken, 0); //Start of shader type declaration line
		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos);
			VOXEL_CORE_ASSERT(eol != std::string::npos, "Syntax error");
			size_t begin = pos + typeTokenLength + 1; // Start of shader type name (after "#type " keyword)
			std::string type = source.substr(begin, eol - begin);
			VOXEL_CORE_ASSERT(shaderTypeFromString(type), "invalid shader type specified");

			size_t nextLinePos = source.find_first_not_of("\r\n", eol); // Start of shader code after shader type declaration line
			VOXEL_CORE_ASSERT(nextLinePos != std::string::npos, "syntax error");
			pos = source.find(typeToken, nextLinePos); // Start of next shader type declaration line

			m_shaderSources[shaderStageFromString(type)] = (pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);
		}

		return m_shaderSources;
	}
	void VulkanShader::compileOrGetVulkanBinaries(const string& filepath, const ShaderSources& shaderSources)
	{
		std::filesystem::path cacheDirectory = getCacheDirectory();

		auto& shaderBinary = m_shaderBinaries;
		for (const auto& [stage, source] : shaderSources)
		{
			std::filesystem::path shaderFilePath = filepath;
			std::filesystem::path cachedPath = cacheDirectory / (shaderFilePath.filename().string() + shaderStageCachedVulkanFileExtension(stage));

			if (std::filesystem::exists(cachedPath))
			{
				VOXEL_CORE_WARN("Shader '{0}' ({1}) is found in cache.", filepath, shaderStageString(stage));
				shaderBinary[stage] = readBinary(cachedPath.string());
			}
			else
			{
				shaderBinary[stage] = compileShaderToSpirv(source, filepath.c_str(), stage);

				std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
				VOXEL_CORE_ASSERT(out.is_open(), "failed to write cache binary file at path " + cachedPath);

				size_t size = shaderBinary[stage].size() * sizeof(uint32);
				out.write((char*)shaderBinary[stage].data(), size);
				out.flush();
				out.close();
			}
		}
	}
	void VulkanShader::createCacheDirectoryIfNeeded()
	{
		std::string cacheDirectory = getCacheDirectory();
		if (!std::filesystem::exists(cacheDirectory))
		{
			VOXEL_CORE_WARN("Shaders cache directory doesn't exists. Creating new folder...");
			std::filesystem::create_directories(cacheDirectory);
		}
	}

	const VkShaderModule VulkanShader::createShaderModule(const std::vector<uint32>& spirv) const
	{
		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = spirv.size() * sizeof(uint32);
		createInfo.pCode = spirv.data();

		VkShaderModule shaderModule;
		VkResult err = vkCreateShaderModule(m_logicalDevice, &createInfo, nullptr, &shaderModule);
		VK_CHECK(err, "failed to create shader module!");
		return shaderModule;
	}
	void VulkanShader::createShader(const ShaderStage& stage, const std::vector<uint32>& spirv)
	{
		VkShaderModule shaderModule = createShaderModule(spirv);
		VkPipelineShaderStageCreateInfo stageInfo = {};
		stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stageInfo.stage = vkInit::shaderStageToVulkanBaseStage(stage);
		stageInfo.module = shaderModule;
		stageInfo.pName = "main";
		m_shaderStages.push_back(stageInfo);
		m_shaderModules.push_back(shaderModule);
	}

	VulkanShader::VulkanShader(const VkDevice& logicalDevice, const char* filepath)
		: m_logicalDevice(logicalDevice)
	{
		VOXEL_ASSERT(filepath, "invalid shader's program file path!");

		createCacheDirectoryIfNeeded();

		const string& shaderProgram = readFile(filepath);
		const ShaderSources& shaderSources = preProcess(shaderProgram);

		m_shaderStages.reserve(shaderSources.size());
		m_shaderModules.reserve(shaderSources.size());
		compileOrGetVulkanBinaries(filepath, shaderSources);
		{
			VoxelEngine::Timer timer;
			for (const auto& [stage, spirv] : m_shaderBinaries)
			{
				createShader(stage, spirv);
			}
			VOXEL_CORE_WARN("Shader '{0}' creation time: {1} ms.", filepath, timer.elapsedTimeInMilliseconds<double>());
		}

		m_shaderSources.clear();
		m_shaderBinaries.clear();
	}
	void VulkanShader::unbind() const
	{
		for (const VkShaderModule& module : m_shaderModules)
		{
			if (module != nullptr)
			{
				vkDestroyShaderModule(m_logicalDevice, module, nullptr);
			}
		}
	}
	VulkanShader::~VulkanShader()
	{
		unbind();
	}
}