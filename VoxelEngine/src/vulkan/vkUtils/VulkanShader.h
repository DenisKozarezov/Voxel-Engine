#pragma once
#include <vulkan/vulkan.h>
#include <core/renderer/Shader.h>

namespace vkUtils
{
	class VulkanShader : public VoxelEngine::renderer::Shader
	{
	private:
		VkShaderModule m_shaderModule;
		VkPipelineShaderStageCreateInfo m_shaderInfo;
		VkDevice m_logicalDevice;

		const VkShaderModule createShaderModule(const std::vector<char>& code) const;
	public:
		VulkanShader(const VkDevice& logicalDevice, const string& filepath, const VkShaderStageFlagBits& shaderType);

		inline VkPipelineShaderStageCreateInfo& getStage() & noexcept { return m_shaderInfo; }

		void unbind() const override;

		~VulkanShader();
	};
}

