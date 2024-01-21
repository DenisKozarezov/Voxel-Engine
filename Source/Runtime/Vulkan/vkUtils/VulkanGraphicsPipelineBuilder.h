#pragma once
#include <Core/CoreTypes.h>
#include <Core/CoreDefines.h>
#include <vulkan/vulkan.h>
#include <vector>

namespace vkInit
{
	struct ShaderPass
	{
		VkPipeline pipeline;
		VkPipelineLayout pipelineLayout;
	};
	
    class VulkanGraphicsPipelineBuilder
    {
    private:
    	VkDevice m_logicalDevice;
    	VkPipelineCreateFlags m_flags = VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT;
    	const VkPipelineVertexInputStateCreateInfo* m_vertexInputInfo = nullptr;
    	const VkPipelineInputAssemblyStateCreateInfo* m_inputAssembly = nullptr;
    	const VkPipelineViewportStateCreateInfo* m_viewportState = nullptr;
    	const VkPipelineRasterizationStateCreateInfo* m_rasterizer = nullptr;
    	const VkPipelineMultisampleStateCreateInfo* m_multisampling = nullptr;
    	const VkPipelineDepthStencilStateCreateInfo* m_depthStencil = nullptr;
    	const VkPipelineDynamicStateCreateInfo* m_dynamicState = nullptr;
    	const VkPipelineShaderStageCreateInfo* m_shaderStages = nullptr;
    	VkPipelineColorBlendStateCreateInfo* m_colorBlending = nullptr;
        const VkPipelineLayoutCreateInfo* m_pipelineLayoutInfo = nullptr;
        VkDescriptorSetLayout m_descriptorSetLayout = VK_NULL_HANDLE;
        VkPipelineCache m_pipelineCache = VK_NULL_HANDLE;
        VkRenderPass m_renderPass = VK_NULL_HANDLE;
        VkPipeline m_basePipelineHandle = VK_NULL_HANDLE;
        int32_t m_basePipelineIndex = 0;
        uint32 m_stagesCount = 0;
        uint32 m_subpass = 0;
    public:
    	VulkanGraphicsPipelineBuilder(const VkDevice& device, const VkPipelineCache cache);
    	
    	VulkanGraphicsPipelineBuilder& setFlags(VkPipelineCreateFlags flags);
    	VulkanGraphicsPipelineBuilder& setStages(const std::vector<VkPipelineShaderStageCreateInfo>& stages);
    	VulkanGraphicsPipelineBuilder& setVertexInputState(const VkPipelineVertexInputStateCreateInfo* pVertexInputState);
    	VulkanGraphicsPipelineBuilder& setInputAssemblyState(const VkPipelineInputAssemblyStateCreateInfo* pInputAssemblyState);
    	VulkanGraphicsPipelineBuilder& setViewportState(const VkPipelineViewportStateCreateInfo* pViewportState);
    	VulkanGraphicsPipelineBuilder& setRasterizationState(const VkPipelineRasterizationStateCreateInfo* pRasterizationState);
    	VulkanGraphicsPipelineBuilder& setMultisampleState(const VkPipelineMultisampleStateCreateInfo* pMultisampleState);
    	VulkanGraphicsPipelineBuilder& setDepthStencilState(const VkPipelineDepthStencilStateCreateInfo* pDepthStencilState);
    	VulkanGraphicsPipelineBuilder& setColorBlendState(const VkPipelineColorBlendStateCreateInfo* pColorBlendState);
    	VulkanGraphicsPipelineBuilder& setColorBlendAttachment(const VkPipelineColorBlendAttachmentState* pColorBlendAttachment);
    	VulkanGraphicsPipelineBuilder& setDynamicState(const VkPipelineDynamicStateCreateInfo* pDynamicState);
    	VulkanGraphicsPipelineBuilder& setLayout(const VkPipelineLayoutCreateInfo* layout);
    	VulkanGraphicsPipelineBuilder& setRenderPass(VkRenderPass renderPass);
    	VulkanGraphicsPipelineBuilder& setSubpass(uint32 subpass);
    	VulkanGraphicsPipelineBuilder& setBasePipelineHandle(VkPipeline basePipelineHandle);
    	VulkanGraphicsPipelineBuilder& setBasePipelineIndex(int32_t basePipelineIndex);

    	NODISCARD ShaderPass build() const;
    };
}