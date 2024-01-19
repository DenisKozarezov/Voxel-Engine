#include "VulkanShader.h"
#include "VulkanValidation.h"
#include "../vkInit/VulkanInitializers.h"
#include <Core/Internal/Timer.h>
#include <Engine/ShaderCompiler/ShaderCompiler.h>

#include "Core/HAL/AssetsManager/Paths.h"

namespace vkUtils
{

	constexpr string VulkanShader::shaderStageCachedVulkanFileExtension(const ShaderStage& stage)
	{
		switch (stage)
		{
		case Vertex:    return ".cached_vulkan.vert";
		case Fragment:  return ".cached_vulkan.frag";
		case Geometry:  return ".cached_vulkan.geom";
		case Compute:	return ".cached_vulkan.compute";
		default:
			RUNTIME_ASSERT(false, "unknown shader stage!");
			return "";
		}
	}

	const utils::shaders::ShaderBinaries VulkanShader::compileOrGetVulkanBinaries(const string& filepath, const utils::shaders::ShaderSources& shaderSources)
	{
		const std::filesystem::path cacheDirectory = getShaderCacheDirectory();

		utils::shaders::ShaderBinaries shaderBinaries;
		for (const auto& [stage, source] : shaderSources)
		{
			const std::filesystem::path shaderFilePath = filepath;
			const std::filesystem::path cachedPath = cacheDirectory / (shaderFilePath.filename().string() + shaderStageCachedVulkanFileExtension(stage));

			const string shaderStageStr = shaderStageString(stage);
			
			if (Paths::fileExists(cachedPath.string()))
			{
				shaderBinaries[stage] = readBinary(cachedPath.string());
				RUNTIME_WARN("Shader '{0}' ({1}) is found in cache.", filepath, shaderStageStr);
			}
			else
			{
				RUNTIME_WARN("Shader '{0}' ({1}) not found in cache. Compiling to SPIR-V...", filepath, shaderStageStr);

				VoxelEngine::Timer timer;
				utils::shaders::ShaderCompiler compiler;
				shaderBinaries[stage] = compiler.compileToSpirv(source, filepath.c_str(), stage);
				RUNTIME_WARN("Shader '{0}' ({1}) compilation time: {2} ms.", filepath, shaderStageStr, timer.elapsedTimeInMilliseconds<double>());

				size_t size = shaderBinaries[stage].size() * sizeof(uint32);
				createCachedBinaryFile(cachedPath.string(), shaderBinaries[stage].data(), size);
			}
		}

		if (!shaderBinaries.empty())
			return shaderBinaries;
		else
			return utils::shaders::ShaderBinaries(0);
	}

	VkShaderModule VulkanShader::createShaderModule(const std::vector<uint32>& spirv) const
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
		const VkShaderModule shaderModule = createShaderModule(spirv);
		VkPipelineShaderStageCreateInfo stageInfo = {};
		stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stageInfo.stage = vkInit::shaderStageToVulkanBaseStage(stage);
		stageInfo.module = shaderModule;
		stageInfo.pName = "main";
		m_shaderStages.push_back(stageInfo);
		m_shaderModules.push_back(shaderModule);
	}

	VulkanShader::VulkanShader(const VkDevice& logicalDevice, const string& filepath)
		: m_logicalDevice(logicalDevice)
	{
		EDITOR_ASSERT(!filepath.empty(), "invalid shader's program file path!");

		createShaderCacheDirectoryIfNeeded();

		const string& shaderProgram = readFile(filepath);
		const auto& shaderSources = utils::shaders::analyzeShaderProgram(shaderProgram);

		m_shaderStages.reserve(shaderSources.size());
		m_shaderModules.reserve(shaderSources.size());
		const auto& shaderBinaries = compileOrGetVulkanBinaries(filepath, shaderSources);
		{
			VoxelEngine::Timer timer;
			for (const auto& [stage, spirv] : shaderBinaries)
			{
				createShader(stage, spirv);
			}
			RUNTIME_WARN("Shader '{0}' creation time: {1} ms.", filepath, timer.elapsedTimeInMilliseconds<double>());
		}
	}

	VulkanShader::~VulkanShader()
	{
		for (const VkShaderModule& module : m_shaderModules)
		{
			if (module != nullptr)
			{
				vkDestroyShaderModule(m_logicalDevice, module, nullptr);
			}
		}
	}
}
