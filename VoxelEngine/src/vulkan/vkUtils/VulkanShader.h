#pragma once
#include <vulkan/vulkan.h>
#include <core/renderer/Shader.h>

namespace vkUtils
{
	using ShaderSources = std::unordered_map<ShaderStage, string>;
	using ShaderBinaries = std::unordered_map<ShaderStage, std::vector<uint32>>;

	class VulkanShader : public VoxelEngine::renderer::Shader
	{
	private:
		VkDevice m_logicalDevice;
		std::vector<VkShaderModule> m_shaderModules = {};
		std::vector<VkPipelineShaderStageCreateInfo> m_shaderStages = {};
		ShaderSources m_shaderSources;
		ShaderBinaries m_shaderBinaries;

		const ShaderSources& preProcess(const std::string& source);
		constexpr const char* shaderStageCachedVulkanFileExtension(const ShaderStage& stage);
		const std::vector<uint32> compileShaderToSpirv(const std::string& shaderProgram, const char* filepath, const ShaderStage& stage);
		void compileOrGetVulkanBinaries(const string& filepath, const ShaderSources& shaderSources);
		void createCacheDirectoryIfNeeded();

		const VkShaderModule createShaderModule(const std::vector<uint32>& spirv) const;
		void createShader(const ShaderStage& stage, const std::vector<uint32>& spirv);
	public:
		VulkanShader() noexcept = delete;
		VulkanShader(const VkDevice& logicalDevice, const char* filepath);

		INLINE std::vector<VkPipelineShaderStageCreateInfo>& getStages() & noexcept { return m_shaderStages; }

		void unbind() const override;

		~VulkanShader();
	};
}

