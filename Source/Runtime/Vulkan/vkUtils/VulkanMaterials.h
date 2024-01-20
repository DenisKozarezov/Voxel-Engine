#pragma once
#include <Renderer/IMaterial.h>
#include "VulkanGraphicsPipelineBuilder.h"

namespace vkUtils
{
#define MATERIALS_MAX 20

	class VulkanMaterial : public VoxelEngine::renderer::IMaterial
	{
	private:
		VkDevice logicalDevice = VK_NULL_HANDLE;
		vkInit::ShaderPass shaderPass;
	public:
		VulkanMaterial(const VkDevice& logicalDevice, const vkInit::ShaderPass& shaderPass,
			const bool& instanced = false)
		: IMaterial(instanced), logicalDevice(logicalDevice), shaderPass(shaderPass) { }
		~VulkanMaterial() override
		{
			vkDestroyPipeline(logicalDevice, shaderPass.pipeline, nullptr);
			vkDestroyPipelineLayout(logicalDevice, shaderPass.pipelineLayout, nullptr);
		}

		void bind() const override;
		void bind(const VkCommandBuffer& commandBuffer, const VkDescriptorSet& descriptorSet) const;
	};

	using MaterialsCache = std::unordered_map<string, VulkanMaterial*>;
	
	const VulkanMaterial* createMaterial(
		const VkDevice& logicalDevice,
		const vkInit::ShaderPass shaderPass,
		const string& matName,
		const bool& instanced = false);
	
	void unregisterMaterial(const string& matName);
	
	const VulkanMaterial* getMaterial(const string& matName);

	void makeMaterials(const VkDevice& device, vkInit::VulkanGraphicsPipelineBuilder& pipelineInfo);
	
	void releaseMaterials();
}

namespace utils
{
	const VoxelEngine::renderer::IMaterial* getMaterial(const string& matName);
}