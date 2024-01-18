#include "VulkanShader.h"
#include "VulkanValidation.h"
#include "../vkInit/VulkanInitializers.h"

#include <Core/Internal/Timer.h>
#include <Engine/ShaderCompiler/ShaderCompiler.h>
#include <Core/HAL/AssetsManager/AssetsProvider.h>

namespace vkUtils
{
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

	const utils::shaders::ShaderBinaries VulkanShader::compileOrGetVulkanBinaries(const string& filepath, const utils::shaders::ShaderSources& shaderSources)
	{
		const std::filesystem::path cacheDirectory = getCacheDirectory();

		utils::shaders::ShaderBinaries shaderBinaries;
		for (const auto& [stage, source] : shaderSources)
		{
			const std::filesystem::path shaderFilePath = filepath;
			const std::filesystem::path cachedPath = cacheDirectory / (shaderFilePath.filename().string() + shaderStageCachedVulkanFileExtension(stage));

			const string shaderStageStr = shaderStageString(stage);
			
			if (std::filesystem::exists(cachedPath))
			{
				shaderBinaries[stage] = readBinary(cachedPath.string());
				VOXEL_CORE_WARN("Shader '{0}' ({1}) is found in cache.", filepath, shaderStageStr);
			}
			else
			{
				VOXEL_CORE_WARN("Shader '{0}' ({1}) not found in cache. Compiling to SPIR-V...", filepath, shaderStageStr);

				VoxelEngine::Timer timer;
				utils::shaders::ShaderCompiler compiler;
				shaderBinaries[stage] = compiler.compileToSpirv(source, filepath.c_str(), stage);
				VOXEL_CORE_WARN("Shader '{0}' ({1}) compilation time: {2} ms.", filepath, shaderStageStr, timer.elapsedTimeInMilliseconds<double>());

				size_t size = shaderBinaries[stage].size() * sizeof(uint32);
				createCachedBinaryFile(cachedPath.string(), shaderBinaries[stage].data(), size);
			}
		}

		if (!shaderBinaries.empty())
			return shaderBinaries;
		else
			return utils::shaders::ShaderBinaries(0);
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
			VOXEL_CORE_WARN("Shader '{0}' creation time: {1} ms.", filepath, timer.elapsedTimeInMilliseconds<double>());
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