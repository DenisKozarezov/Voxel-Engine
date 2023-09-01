#pragma once
#include <vulkan/vulkan.h>
#include <core/PrimitiveTypes.h>

namespace vkUtils
{
	struct VulkanPipelineCreateInfo
	{
		VkPipelineCreateFlags flags = VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT;
		VkPipelineVertexInputStateCreateInfo* vertexInputInfo;
		VkPipelineInputAssemblyStateCreateInfo* inputAssembly;
		VkPipelineViewportStateCreateInfo* viewportState;
		VkPipelineRasterizationStateCreateInfo* rasterizer;
		VkPipelineMultisampleStateCreateInfo* multisampling;
		VkPipelineColorBlendAttachmentState* colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo* colorBlending;
		VkPipelineDepthStencilStateCreateInfo* depthStencil;
		VkPipelineDynamicStateCreateInfo* dynamicState;
		VkDescriptorSetLayout* descriptorSetLayout;
		VkRenderPass* renderPass;
		VkPipelineShaderStageCreateInfo* shaderStages;
		VkPipeline basePipelineHandle;
		VkPipelineLayoutCreateInfo pipelineLayoutInfo;
		uint32 stagesCount;

		void build(
			const VkDevice& logicalDevice,
			const VkPipelineLayout& pipelineLayout,
			const VkPipelineCache& pipelineCache,
			VkPipeline* pipeline);
	};

	class VulkanMaterial
	{
	public:
		VkPipeline pipeline = VK_NULL_HANDLE;
		VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
		bool instanced = false;

		void bind(const VkCommandBuffer& commandBuffer) const;
	};

	const VulkanMaterial* createMaterial(const VkPipeline& matPipeline, const VkPipelineLayout& matLayout, const string& matName);
	const VulkanMaterial* getMaterial(const string& matName);
	void makeMaterials(
		const VkDevice& logicalDevice, 
		VulkanPipelineCreateInfo& pipelineInfo,
		const VkPipelineCache& pipelineCache);
	void releaseMaterials(const VkDevice& logicalDevice);
}