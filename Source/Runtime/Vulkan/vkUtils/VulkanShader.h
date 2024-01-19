#pragma once
#include <Engine/ShaderCompiler/ShaderLexer.h>
#include <vulkan/vulkan.h>

namespace vkUtils
{
	class VulkanShader : public VoxelEngine::renderer::Shader
	{
	private:
		VkDevice m_logicalDevice;
		std::vector<VkShaderModule> m_shaderModules = {};
		std::vector<VkPipelineShaderStageCreateInfo> m_shaderStages = {};

		constexpr string shaderStageCachedVulkanFileExtension(const ShaderStage& stage);
		const utils::shaders::ShaderBinaries compileOrGetVulkanBinaries(const string& filepath, const utils::shaders::ShaderSources& shaderSources);

		VkShaderModule createShaderModule(const std::vector<uint32>& spirv) const;
		void createShader(const ShaderStage& stage, const std::vector<uint32>& spirv);
	public:
		VulkanShader() noexcept = delete;
		VulkanShader(const VkDevice& logicalDevice, const string& filepath);

		FORCE_INLINE std::vector<VkPipelineShaderStageCreateInfo>& getStages() & noexcept { return m_shaderStages; }
		
		~VulkanShader() override;
	};
}

