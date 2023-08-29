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
	VulkanShader::VulkanShader(const VkDevice& logicalDevice, const char* filepath, const ShaderStage& shaderStage)
		: m_logicalDevice(logicalDevice)
	{
		VOXEL_ASSERT(filepath, "invalid shader's program file path!");

		const string shaderProgram = readFile(filepath);

		VkShaderModule shaderModule = createShaderModule(shaderProgram);
		VkPipelineShaderStageCreateInfo stageInfo = {};
		stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stageInfo.stage = vkInit::shaderStageToVulkanBaseStage(shaderStage);
		stageInfo.module = shaderModule;
		stageInfo.pName = "main";
		m_shaderStages[0] = stageInfo;
		m_shaderModules[0] = shaderModule;
	}
	VulkanShader::VulkanShader(const VkDevice& logicalDevice, const char* vertexPath, const char* fragmentPath, const char* geomertryPath)
		: m_logicalDevice(logicalDevice)
	{
		// Vertex shader stage
		string shaderProgram = readFile(vertexPath);

		VkShaderModule vertexModule = createShaderModule(shaderProgram);
		VkPipelineShaderStageCreateInfo vertexStage = {};
		vertexStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertexStage.stage = vkInit::shaderStageToVulkanBaseStage(Vertex);
		vertexStage.module = vertexModule;
		vertexStage.pName = "main";
		m_shaderStages[0] = vertexStage;
		m_shaderModules[0] = vertexModule;

		// Fragment shader stage
		if (fragmentPath != nullptr)
		{
			string shaderProgram = readFile(fragmentPath);

			VkShaderModule fragmentModule = createShaderModule(shaderProgram);
			VkPipelineShaderStageCreateInfo fragmentStage = {};
			fragmentStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			fragmentStage.stage = vkInit::shaderStageToVulkanBaseStage(Fragment);
			fragmentStage.module = fragmentModule;
			fragmentStage.pName = "main";
			m_shaderStages[1] = fragmentStage;
			m_shaderModules[1] = fragmentModule;
		}

		// Geometry shader stage
		if (geomertryPath != nullptr)
		{
			string shaderProgram = readFile(geomertryPath);

			VkShaderModule geometryModule = createShaderModule(shaderProgram);
			VkPipelineShaderStageCreateInfo geometryStage = {};
			geometryStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			geometryStage.stage = vkInit::shaderStageToVulkanBaseStage(Geometry);
			geometryStage.module = geometryModule;
			geometryStage.pName = "main";
			m_shaderStages[2] = geometryStage;
			m_shaderModules[2] = geometryModule;
		}
	}
	void VulkanShader::unbind() const
	{
		for (const VkShaderModule& module : m_shaderModules)
		{
			if (module != nullptr)
			{
				vkDestroyShaderModule(m_logicalDevice, module, nullptr);
			}
		}
	}
	VulkanShader::~VulkanShader()
	{
		unbind();
	}
}