#include "VulkanShader.h"
#include "VulkanValidation.h"

namespace vkUtils
{
	const VkShaderModule VulkanShader::createShaderModule(const std::vector<char>& code) const
	{
		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32*>(code.data());

		VkShaderModule shaderModule;
		VkResult err = vkCreateShaderModule(_logicalDevice, &createInfo, nullptr, &shaderModule);
		vkUtils::check_vk_result(err, "failed to create shader module!");
		return shaderModule;
	}	
	VulkanShader::VulkanShader(const VkDevice& logicalDevice, const string& filepath, const VkShaderStageFlagBits& shaderType) 
		: Shader(filepath), 
		_logicalDevice(logicalDevice)
	{
		auto shaderProgram = readFile(filepath);

		_shaderModule = createShaderModule(shaderProgram);
		_shaderInfo = {};
		_shaderInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		_shaderInfo.stage = shaderType;
		_shaderInfo.module = _shaderModule;
		_shaderInfo.pName = "main";
	}
	void VulkanShader::unbind() const
	{
		vkDestroyShaderModule(_logicalDevice, _shaderModule, nullptr);
	}
}