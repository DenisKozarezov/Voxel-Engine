#include "VulkanShader.h"
#include "VulkanValidation.h"
#include "../vkInit/VulkanInitializers.h"
#include "../VulkanBackend.h"

namespace vkUtils
{
	const VkShaderModule VulkanShader::createShaderModule(const string& code) const
	{
		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32*>(code.data());

		VkShaderModule shaderModule;
		VkResult err = vkCreateShaderModule(m_logicalDevice, &createInfo, nullptr, &shaderModule);
		VK_CHECK(err, "failed to create shader module!");
		return shaderModule;
	}	
	VulkanShader::VulkanShader(const VkDevice& logicalDevice, const string& filepath, const ShaderStage& shaderStage)
		: Shader(filepath, shaderStage),
		m_logicalDevice(logicalDevice)
	{
		const string shaderProgram = readFile(filepath);

		m_shaderModule = createShaderModule(shaderProgram);
		m_shaderInfo = {};
		m_shaderInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		m_shaderInfo.stage = vkInit::shaderStageToVulkanBaseStage(shaderStage);
		m_shaderInfo.module = m_shaderModule;
		m_shaderInfo.pName = "main";
	}
	void VulkanShader::unbind() const
	{
		vkDestroyShaderModule(m_logicalDevice, m_shaderModule, nullptr);
	}
	VulkanShader::~VulkanShader()
	{
		unbind();
	}
}