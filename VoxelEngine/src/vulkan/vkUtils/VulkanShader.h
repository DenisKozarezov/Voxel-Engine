#pragma once
#include <core/utils/ShaderLexer.h>
#include <vulkan/vulkan.h>

namespace vkUtils
{
	class VulkanShader : public VoxelEngine::renderer::Shader
	{
	private:
		VkDevice m_logicalDevice;
		std::vector<VkShaderModule> m_shaderModules = {};
		std::vector<VkPipelineShaderStageCreateInfo> m_shaderStages = {};

		constexpr const char* shaderStageCachedVulkanFileExtension(const ShaderStage& stage);
		const std::vector<uint32> compileShaderToSpirv(const string& shaderProgram, const char* filepath, const ShaderStage& stage);
		const utils::lexer::ShaderBinaries compileOrGetVulkanBinaries(const string& filepath, const utils::lexer::ShaderSources& shaderSources);
		void createCacheDirectoryIfNeeded();

		const VkShaderModule createShaderModule(const std::vector<uint32>& spirv) const;
		void createShader(const ShaderStage& stage, const std::vector<uint32>& spirv);
	public:
		VulkanShader() noexcept = delete;
		VulkanShader(const VkDevice& logicalDevice, const char* filepath);

		INLINE std::vector<VkPipelineShaderStageCreateInfo>& getStages() & noexcept { return m_shaderStages; }

		void unbind() const override;

		~VulkanShader() override;
	};
}

