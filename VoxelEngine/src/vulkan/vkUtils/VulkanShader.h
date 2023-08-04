#pragma once
#include <vulkan/vulkan.h>
#include <core/renderer/Shader.h>

namespace vkUtils
{
	class VulkanShader : public VoxelEngine::renderer::Shader
	{
	private:
		VkShaderModule _shaderModule;
		VkPipelineShaderStageCreateInfo _shaderInfo;
		VkDevice _logicalDevice;

		const VkShaderModule createShaderModule(const std::vector<char>& code) const;
	public:
		VulkanShader(const VkDevice& logicalDevice, const string& filepath, const VkShaderStageFlagBits& shaderType);

		inline VkShaderModule& getShaderModule() & noexcept { return _shaderModule; }
		inline constexpr VkPipelineShaderStageCreateInfo& getStage() & noexcept { return _shaderInfo; }

		void unbind() const override;

		~VulkanShader();
	};
}

