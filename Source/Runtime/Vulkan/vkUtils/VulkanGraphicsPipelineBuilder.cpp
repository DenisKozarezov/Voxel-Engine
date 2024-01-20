#include "VulkanGraphicsPipelineBuilder.h"
#include <Vulkan/vkInit/VulkanInitializers.h>
#include <Vulkan/vkUtils/VulkanValidation.h>

namespace vkInit
{
    VulkanGraphicsPipelineBuilder::VulkanGraphicsPipelineBuilder(const VkDevice& device, VkPipelineCache cache)
        : m_logicalDevice(device), m_pipelineCache(cache)
    {
       
    }

    VulkanGraphicsPipelineBuilder& VulkanGraphicsPipelineBuilder::setFlags(VkPipelineCreateFlags flags)
    {
        m_flags = flags;
        return *this;
    }

    VulkanGraphicsPipelineBuilder& VulkanGraphicsPipelineBuilder::setStages(
        const std::vector<VkPipelineShaderStageCreateInfo>& stages)
    {
        m_shaderStages = stages.data();
        m_stagesCount = static_cast<uint32>(stages.size());
        return *this;
    }

    VulkanGraphicsPipelineBuilder& VulkanGraphicsPipelineBuilder::setVertexInputState(
        const VkPipelineVertexInputStateCreateInfo* pVertexInputState)
    {
        m_vertexInputInfo = pVertexInputState;
        return *this;
    }

    VulkanGraphicsPipelineBuilder& VulkanGraphicsPipelineBuilder::setInputAssemblyState(
        const VkPipelineInputAssemblyStateCreateInfo* pInputAssemblyState)
    {
        m_inputAssembly = pInputAssemblyState;
        return *this;
    }

    VulkanGraphicsPipelineBuilder& VulkanGraphicsPipelineBuilder::setViewportState(
        const VkPipelineViewportStateCreateInfo* pViewportState)
    {
        m_viewportState = pViewportState;
        return *this;
    }

    VulkanGraphicsPipelineBuilder& VulkanGraphicsPipelineBuilder::setRasterizationState(
        const VkPipelineRasterizationStateCreateInfo* pRasterizationState)
    {
        m_rasterizer = pRasterizationState;
        return *this;
    }

    VulkanGraphicsPipelineBuilder& VulkanGraphicsPipelineBuilder::setMultisampleState(
        const VkPipelineMultisampleStateCreateInfo* pMultisampleState)
    {
        m_multisampling = pMultisampleState;
        return *this;
    }

    VulkanGraphicsPipelineBuilder& VulkanGraphicsPipelineBuilder::setDepthStencilState(
        const VkPipelineDepthStencilStateCreateInfo* pDepthStencilState)
    {
        m_depthStencil = pDepthStencilState;
        return *this;
    }

    VulkanGraphicsPipelineBuilder& VulkanGraphicsPipelineBuilder::setColorBlendState(
        const VkPipelineColorBlendStateCreateInfo* pColorBlendState)
    {
        m_colorBlending = const_cast<VkPipelineColorBlendStateCreateInfo*>(pColorBlendState);
        return *this;
    }

    VulkanGraphicsPipelineBuilder& VulkanGraphicsPipelineBuilder::setColorBlendAttachment(
        const VkPipelineColorBlendAttachmentState* pColorBlendAttachment)
    {
        m_colorBlending->pAttachments = pColorBlendAttachment;
        return *this;
    }

    VulkanGraphicsPipelineBuilder& VulkanGraphicsPipelineBuilder::setDynamicState(
        const VkPipelineDynamicStateCreateInfo* pDynamicState)
    {
        m_dynamicState = pDynamicState;
        return *this;
    }

    VulkanGraphicsPipelineBuilder& VulkanGraphicsPipelineBuilder::setLayout(VkPipelineLayoutCreateInfo layout)
    {
        m_pipelineLayoutInfo = layout;
        return *this;
    }

    VulkanGraphicsPipelineBuilder& VulkanGraphicsPipelineBuilder::setRenderPass(VkRenderPass renderPass)
    {
        m_renderPass = renderPass;
        return *this;
    }

    VulkanGraphicsPipelineBuilder& VulkanGraphicsPipelineBuilder::setSubpass(uint32_t subpass)
    {
        m_subpass = subpass;
        return *this;
    }

    VulkanGraphicsPipelineBuilder& VulkanGraphicsPipelineBuilder::setBasePipelineHandle(VkPipeline basePipelineHandle)
    {
        m_basePipelineHandle = basePipelineHandle;
        return *this;
    }

    VulkanGraphicsPipelineBuilder& VulkanGraphicsPipelineBuilder::setBasePipelineIndex(int32_t basePipelineIndex)
    {
        m_basePipelineIndex = basePipelineIndex;
        return *this;
    }

    ShaderPass VulkanGraphicsPipelineBuilder::build() const
    {
        VkPipelineLayout pipelineLayout;
        VkResult err = vkCreatePipelineLayout(m_logicalDevice, &m_pipelineLayoutInfo, nullptr, &pipelineLayout);
        VK_CHECK(err, "failed to create pipeline layout!");
        
        VkGraphicsPipelineCreateInfo pipelineInfo = vkInit::pipelineCreateInfo();
        pipelineInfo.flags = m_flags;
        pipelineInfo.pVertexInputState = m_vertexInputInfo;
        pipelineInfo.pInputAssemblyState = m_inputAssembly;
        pipelineInfo.pViewportState = m_viewportState;
        pipelineInfo.pRasterizationState = m_rasterizer;
        pipelineInfo.pMultisampleState = m_multisampling;
        pipelineInfo.pColorBlendState = m_colorBlending;
        pipelineInfo.pDepthStencilState = m_depthStencil;
        pipelineInfo.pDynamicState = m_dynamicState;
        pipelineInfo.layout = pipelineLayout;
        pipelineInfo.renderPass = m_renderPass;
        pipelineInfo.subpass = m_subpass;
        pipelineInfo.pStages = m_shaderStages;
        pipelineInfo.stageCount = m_stagesCount;
        pipelineInfo.basePipelineHandle = m_basePipelineHandle;

        VkPipeline pipeline;
        err = vkCreateGraphicsPipelines(m_logicalDevice, m_pipelineCache, 1, &pipelineInfo, nullptr, &pipeline);
        VK_CHECK(err, "failed to create graphics pipeline!");

        const ShaderPass shaderPass = {
            .pipeline = pipeline,
            .pipelineLayout = pipelineLayout
        };
        return shaderPass;
    }
}
