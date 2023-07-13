#pragma once
#include <vulkan/vulkan.h>
#include <core/renderer/Shader.h>

namespace vulkan::shaders
{
	class VulkanShader : public VoxelEngine::renderer::Shader
	{
	private:
		VkShaderModule _shaderModule;
		VkPipelineShaderStageCreateInfo _shaderInfo;
		VkDevice _logicalDevice;

		const VkShaderModule createShaderModule(const std::vector<char>& code) const;
	public:
		VulkanShader(const string& filepath, const VkShaderStageFlagBits& shaderType);

		inline const VkShaderModule& getShaderModule() & { return _shaderModule; }
		inline const VkPipelineShaderStageCreateInfo& getStage() & { return _shaderInfo; }
		void setUniform(const string& name, const glm::vec2 vector) const override;
		void setUniform(const string& name, const glm::vec3 vector) const override;
		void setUniform(const string& name, const glm::vec4 vector) const override;
		void setUniform(const string& name, const glm::mat4 matrix) const override;
		void setUniform(const string& name, const float value) const override;
		void unbind() const override;
	};
}

