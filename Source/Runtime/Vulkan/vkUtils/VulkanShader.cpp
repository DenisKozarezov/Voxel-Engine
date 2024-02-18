#include "VulkanShader.h"
#include "VulkanValidation.h"
#include "../vkInit/VulkanInitializers.h"
#include <Core/Internal/Timer.h>
#include <Core/HAL/AssetsManager/Paths.h>
#include <Core/HAL/AssetsManager/Assets/ShaderAsset.h>
#include <Engine/ShaderCompiler/ShaderCompiler.h>

namespace vkUtils
{

	constexpr string VulkanShader::shaderStageCachedVulkanFileExtension(const ShaderStage& stage)
	{
		switch (stage)
		{
		case ShaderStage::Vertex:    return ".cached_vulkan.vert";
		case ShaderStage::Fragment:  return ".cached_vulkan.frag";
		case ShaderStage::Geometry:  return ".cached_vulkan.geom";
		case ShaderStage::Compute:	return ".cached_vulkan.compute";
		default:
			RUNTIME_ASSERT(false, "unknown shader stage!");
			return "";
		}
	}

	utils::shaders::ShaderBinaries VulkanShader::compileOrGetVulkanBinaries(const string& filepath, const utils::shaders::ShaderSources& shaderSources)
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
				const auto desc = assets::AssetDescription(cachedPath.string());
				auto asset = assets::ShaderAsset(desc);
				asset.loadFromFile();
				shaderBinaries[stage] = asset.readBinaries();
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
	}

	VulkanShader::VulkanShader(const VkDevice& logicalDevice, const string& filepath)
		: m_logicalDevice(logicalDevice)
	{
		RUNTIME_ASSERT(!filepath.empty(), "invalid shader's program file path!");

		createShaderCacheDirectoryIfNeeded();

		const auto desc = assets::AssetDescription(filepath);
		auto asset = assets::ShaderAsset(desc);
		asset.loadFromFile();
		
		const auto& shaderSources = utils::shaders::analyzeShaderProgram(asset.readProgram());

		m_shaderStages.reserve(shaderSources.size());
		const auto& shaderBinaries = compileOrGetVulkanBinaries(filepath, shaderSources);
		{
			const VoxelEngine::Timer timer;
			for (const auto& [stage, spirv] : shaderBinaries)
			{
				createShader(stage, spirv);
			}
			RUNTIME_WARN("Shader '{0}' creation time: {1} ms.", filepath, timer.elapsedTimeInMilliseconds<double>());
		}
	}

	VulkanShader::~VulkanShader()
	{
		for (const auto& stage : m_shaderStages)
		{
			if (stage.module)
			{
				vkDestroyShaderModule(m_logicalDevice, stage.module, nullptr);
			}
		}
		m_shaderStages.clear();
	}
}
