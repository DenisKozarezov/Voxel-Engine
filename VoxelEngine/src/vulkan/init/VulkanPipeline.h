#pragma once
#include "VulkanInitializers.h"
#include "../utils/VulkanValidation.h"
#include "../utils/VulkanShader.h"
#include "../VulkanVertex.h"

namespace vulkan
{
	struct GraphicsPipilineInputBundle
	{
		VkDevice logicalDevice;
		VkRenderPass& renderPass;
		VkDescriptorSetLayout descriptorSetLayout;
		string vertexFilepath;
		string fragmentFilepath;
	};

	struct GraphicsPipelineOutBundle
	{
		VkPipeline pipeline;
		VkPipelineLayout layout;
	};

	const VkRenderPass createRenderPass(const VkDevice& logicalDevice, const VkFormat& swapChainImageFormat)
	{
		VkAttachmentDescription colorAttachment = initializers::renderPassColorAttachment(swapChainImageFormat);

		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;

		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		std::vector<VkAttachmentDescription> attachments = { colorAttachment };
		std::vector<VkSubpassDescription> subpasses = { subpass };
		std::vector<VkSubpassDependency> dependencies = { dependency };
		VkRenderPassCreateInfo renderPassInfo = initializers::renderPassCreateInfo(attachments, subpasses, dependencies);

		VkRenderPass renderPass;
		VkResult err = vkCreateRenderPass(logicalDevice, &renderPassInfo, nullptr, &renderPass);
		check_vk_result(err, "failed to create render pass!");

		VOXEL_CORE_TRACE("Vulkan render pass created.")

		return renderPass;
	}
	
	const GraphicsPipelineOutBundle createGraphicsPipeline(const GraphicsPipilineInputBundle& inputBundle)
	{
		shaders::VulkanShader vertexShader = shaders::VulkanShader(inputBundle.logicalDevice, inputBundle.vertexFilepath, VK_SHADER_STAGE_VERTEX_BIT);
		shaders::VulkanShader fragShader = shaders::VulkanShader(inputBundle.logicalDevice, inputBundle.fragmentFilepath, VK_SHADER_STAGE_FRAGMENT_BIT);

		std::vector<VkPipelineShaderStageCreateInfo> shaderStages = { vertexShader.getStage(), fragShader.getStage() };
		const auto& bindingDescription = Vertex::getBindingDescription();
		const auto& attributeDescription = Vertex::getAttributeDescriptions();

		VkPipelineVertexInputStateCreateInfo vertexInputInfo = initializers::pipelineVertexInputStateCreateInfo(
			&bindingDescription,
			1,
			attributeDescription.data(),
			static_cast<uint32>(attributeDescription.size()));

		VkPipelineInputAssemblyStateCreateInfo inputAssembly = initializers::pipelineInputAssemblyStateCreateInfo(
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			VK_FALSE);

		VkPipelineViewportStateCreateInfo viewportState = initializers::pipelineViewportStateCreateInfo(1, 1);

		VkPipelineRasterizationStateCreateInfo rasterizer = initializers::pipelineRasterizationStateCreateInfo(
			VK_POLYGON_MODE_FILL,
			VK_CULL_MODE_BACK_BIT,
			VK_FRONT_FACE_CLOCKWISE);

		VkPipelineMultisampleStateCreateInfo multisampling = initializers::pipelineMultisampleStateCreateInfo(VK_SAMPLE_COUNT_1_BIT);

		VkPipelineColorBlendAttachmentState colorBlendAttachment = initializers::pipelineColorBlendAttachmentState(
			VK_COLOR_COMPONENT_R_BIT |
			VK_COLOR_COMPONENT_G_BIT |
			VK_COLOR_COMPONENT_B_BIT |
			VK_COLOR_COMPONENT_A_BIT,
			VK_FALSE);
		VkPipelineColorBlendStateCreateInfo colorBlending = initializers::pipelineColorBlendStateCreateInfo(colorBlendAttachment);

		std::vector<VkDynamicState> dynamicStates =
		{
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};
		VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = initializers::pipelineDynamicStateCreateInfo(dynamicStates);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo = initializers::pipelineLayoutCreateInfo(&inputBundle.descriptorSetLayout);

		VkPipelineLayout pipelineLayout;
		VkResult err = vkCreatePipelineLayout(inputBundle.logicalDevice, &pipelineLayoutInfo, nullptr, &pipelineLayout);
		check_vk_result(err, "failed to create pipeline layout!");

		VkGraphicsPipelineCreateInfo pipelineInfo = initializers::pipelineCreateInfo();
		pipelineInfo.stageCount = shaderStages.size();
		pipelineInfo.pStages = shaderStages.data();
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = &dynamicStateCreateInfo;
		pipelineInfo.layout = pipelineLayout;
		pipelineInfo.renderPass = inputBundle.renderPass;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

		VkPipeline pipeline;
		err = vkCreateGraphicsPipelines(inputBundle.logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline);
		check_vk_result(err, "failed to create graphics pipeline!");

		vertexShader.unbind();
		fragShader.unbind();

		VOXEL_CORE_TRACE("Vulkan graphics pipeline created.")

		GraphicsPipelineOutBundle bundle =
		{
			.pipeline = pipeline,
			.layout = pipelineLayout
		};
		return bundle;
	}
}