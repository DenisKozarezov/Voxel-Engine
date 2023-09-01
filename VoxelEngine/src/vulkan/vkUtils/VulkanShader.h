#pragma once
#include <vulkan/vulkan.h>
#include <core/renderer/Shader.h>

namespace vkUtils
{
	class VulkanShader : public VoxelEngine::renderer::Shader
	{
	private:
		VkDevice m_logicalDevice;
		std::array<VkShaderModule, 3> m_shaderModules = {};
		std::array<VkPipelineShaderStageCreateInfo, 3> m_shaderStages = {};

		const VkShaderModule createShaderModule(const string& code) const;
	public:
		VulkanShader() noexcept = delete;
		VulkanShader(const VkDevice& logicalDevice, const char* filepath, const ShaderStage& shaderStage);
		VulkanShader(const VkDevice& logicalDevice, const char* vertexPath, const char* fragmentPath, const char* geomertryPath = nullptr);

		INLINE std::array<VkPipelineShaderStageCreateInfo, 3>& getStages() & noexcept { return m_shaderStages; }

		void unbind() const override;

		~VulkanShader();
	};
}

