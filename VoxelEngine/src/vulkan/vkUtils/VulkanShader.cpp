#include "VulkanShader.h"
#include "VulkanValidation.h"
#include "../vkInit/VulkanInitializers.h"
#include "../VulkanBackend.h"

#include <core/Timer.h>
#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>
#include <assets_management/AssetsProvider.h>

namespace vkUtils
{
	constexpr ShaderStage shaderTypeFromString(const std::string& type)
	{
		if (type == "vertex")
			return Vertex;
		if (type == "fragment")
			return Fragment;
		if (type == "geometry")
			return Geometry;
		if (type == "compute")
			return Compute;

		VOXEL_CORE_ASSERT(false, "Unknown shader stage!");
		return None;
	}
	constexpr shaderc_shader_kind shaderStageToShaderStage(const ShaderStage& stage)
	{
		switch (stage)
		{
		case Vertex:	return shaderc_glsl_vertex_shader;
		case Fragment:	return shaderc_glsl_fragment_shader;
		case Geometry:	return shaderc_glsl_geometry_shader;
		case Compute:	return shaderc_glsl_compute_shader;
		}
		VOXEL_CORE_ASSERT(false, "Unknown shader stage!");
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
			VOXEL_CORE_ASSERT(false, "Unknown shader stage!");
			return nullptr;
		}
	}
	ShaderSources VulkanShader::preProcess(const std::string& source)
	{
		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = source.find(typeToken, 0); //Start of shader type declaration line
		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos);
			VOXEL_CORE_ASSERT(eol != std::string::npos, "Syntax error");
			size_t begin = pos + typeTokenLength + 1; //Start of shader type name (after "#type " keyword)
			std::string type = source.substr(begin, eol - begin);
			VOXEL_CORE_ASSERT(shaderTypeFromString(type), "Invalid shader type specified");

			size_t nextLinePos = source.find_first_not_of("\r\n", eol); //Start of shader code after shader type declaration line
			VOXEL_CORE_ASSERT(nextLinePos != std::string::npos, "Syntax error");
			pos = source.find(typeToken, nextLinePos); //Start of next shader type declaration line

			m_shaderSources[shaderTypeFromString(type)] = (pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);
		}

		return m_shaderSources;
	}
	void VulkanShader::compileOrGetVulkanBinaries(const string& filepath, const ShaderSources& shaderSources)
	{
		std::filesystem::path cacheDirectory = getCacheDirectory();

		auto& shaderData = m_shaderBinaries;
		for (const auto& [stage, source] : shaderSources)
		{
			std::filesystem::path shaderFilePath = filepath;
			std::filesystem::path cachedPath = cacheDirectory / (shaderFilePath.filename().string() + shaderStageCachedVulkanFileExtension(stage));

			if (std::filesystem::exists(cachedPath))
			{
				shaderData[stage] = readFile(cachedPath.string());
			}
			else
			{
				VoxelEngine::Timer timer;

				shaderc::Compiler compiler;
				shaderc::CompileOptions options;
				options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
				const bool optimize = true;
				if (optimize)
					options.SetOptimizationLevel(shaderc_optimization_level_performance);

				shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, shaderStageToShaderStage(stage), filepath.c_str(), options);
				VOXEL_CORE_ASSERT(module.GetCompilationStatus() == shaderc_compilation_status_success, module.GetErrorMessage());

				shaderData[stage] = std::string(module.cbegin(), module.cend());

				std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
				if (out.is_open())
				{
					auto& data = shaderData[stage];
					out.write((char*)data.data(), data.size());
					out.flush();
					out.close();
				}

				VOXEL_CORE_WARN("Shader '{0}' compilation time: {1} ms.", filepath, timer.elapsedTimeInMilliseconds<double>());
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

	const VkShaderModule VulkanShader::createShaderModule(const string& spirv) const
	{
		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = spirv.size();
		createInfo.pCode = reinterpret_cast<const uint32*>(spirv.data());

		VkShaderModule shaderModule;
		VkResult err = vkCreateShaderModule(m_logicalDevice, &createInfo, nullptr, &shaderModule);
		VK_CHECK(err, "failed to create shader module!");
		return shaderModule;
	}
	void VulkanShader::createShader(const ShaderStage& stage, const string& spirv)
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

		m_shaderStages.reserve(3);
		m_shaderModules.reserve(3);

		const string& shaderProgram = readFile(filepath);
		const ShaderSources& shaderSources = preProcess(shaderProgram);

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
	VulkanShader::VulkanShader(const VkDevice& logicalDevice, const char* vertexPath, const char* fragmentPath, const char* geometryPath)
		: m_logicalDevice(logicalDevice)
	{
		VoxelEngine::Timer timer;
		m_shaderStages.reserve(3);
		m_shaderModules.reserve(3);

		string vertexSpirv = readFile(vertexPath);
		createShader(Vertex, vertexSpirv);
		VOXEL_CORE_WARN("Shader '{0}' creation time: {1} ms.", vertexPath, timer.elapsedTimeInMilliseconds<double>());

		if (fragmentPath != nullptr)
		{
			timer.reset();
			string fragmentSpirv = readFile(fragmentPath);
			createShader(Fragment, fragmentSpirv);
			VOXEL_CORE_WARN("Shader '{0}' creation time: {1} ms.", fragmentPath, timer.elapsedTimeInMilliseconds<double>());
		}

		if (geometryPath != nullptr)
		{
			timer.reset();
			string geometrySpirv = readFile(geometryPath);
			createShader(Geometry, geometrySpirv);
			VOXEL_CORE_WARN("Shader '{0}' creation time: {1} ms.", geometryPath, timer.elapsedTimeInMilliseconds<double>());
		}
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