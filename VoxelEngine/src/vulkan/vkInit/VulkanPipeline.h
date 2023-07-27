#pragma once
#include "VulkanInitializers.h"
#include "../vkUtils/VulkanValidation.h"
#include "../vkUtils/VulkanShader.h"
#include "../VulkanVertex.h"

namespace vkInit
{
	struct GraphicsPipilineInputBundle
	{
		VkDevice logicalDevice;
		VkRenderPass& renderPass;
		VkSampleCountFlagBits msaaSamples;
		VkDescriptorSetLayout descriptorSetLayout;
		string vertexFilepath;
		string fragmentFilepath;
	};

	struct GraphicsPipelineOutBundle
	{
		VkPipeline pipeline;
		VkPipelineLayout layout;
	};

	const VkRenderPass createRenderPass(
		const VkDevice& logicalDevice, 
		const VkFormat& swapChainImageFormat, 
		const VkFormat depthFormat,
		const VkSampleCountFlagBits& msaaSamples)
	{
		VkAttachmentDescription colorAttachmentResolve = renderPassColorAttachment(swapChainImageFormat, VK_SAMPLE_COUNT_1_BIT);
		colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;

		VkAttachmentDescription colorAttachment = renderPassColorAttachment(swapChainImageFormat, msaaSamples);
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentDescription depthAttachment = renderPassDepthAttachment(depthFormat, msaaSamples);

		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentRef{};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		
		VkAttachmentReference colorAttachmentResolveRef{};
		colorAttachmentResolveRef.attachment = 2;
		colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		subpass.pDepthStencilAttachment = &depthAttachmentRef;
		subpass.pResolveAttachments = &colorAttachmentResolveRef;

		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		std::vector<VkAttachmentDescription> attachments = { colorAttachment, depthAttachment, colorAttachment };
		std::vector<VkSubpassDescription> subpasses = { subpass };
		std::vector<VkSubpassDependency> dependencies = { dependency };
		VkRenderPassCreateInfo renderPassInfo = renderPassCreateInfo(attachments, subpasses, dependencies);

		VkRenderPass renderPass;
		VkResult err = vkCreateRenderPass(logicalDevice, &renderPassInfo, nullptr, &renderPass);
		vkUtils::check_vk_result(err, "failed to create render pass!");

		VOXEL_CORE_TRACE("Vulkan render pass created.")

		return renderPass;
	}
	
	const GraphicsPipelineOutBundle createGraphicsPipeline(const GraphicsPipilineInputBundle& inputBundle)
	{
		vkUtils::VulkanShader vertexShader = vkUtils::VulkanShader(inputBundle.logicalDevice, inputBundle.vertexFilepath, VK_SHADER_STAGE_VERTEX_BIT);
		vkUtils::VulkanShader fragShader = vkUtils::VulkanShader(inputBundle.logicalDevice, inputBundle.fragmentFilepath, VK_SHADER_STAGE_FRAGMENT_BIT);

		std::vector<VkPipelineShaderStageCreateInfo> shaderStages = { vertexShader.getStage(), fragShader.getStage() };
		const auto& bindingDescription = vulkan::Vertex::getBindingDescription();
		const auto& attributeDescription = vulkan::Vertex::getAttributeDescriptions();

		VkPipelineVertexInputStateCreateInfo vertexInputInfo = pipelineVertexInputStateCreateInfo(
			&bindingDescription,
			1,
			attributeDescription.data(),
			static_cast<uint32>(attributeDescription.size()));

		VkPipelineInputAssemblyStateCreateInfo inputAssembly = pipelineInputAssemblyStateCreateInfo(
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			VK_FALSE);

		VkPipelineViewportStateCreateInfo viewportState = pipelineViewportStateCreateInfo(1, 1);

		VkPipelineRasterizationStateCreateInfo rasterizer = pipelineRasterizationStateCreateInfo(
			VK_POLYGON_MODE_FILL,
			VK_CULL_MODE_BACK_BIT,
			VK_FRONT_FACE_COUNTER_CLOCKWISE);

		VkPipelineMultisampleStateCreateInfo multisampling = pipelineMultisampleStateCreateInfo(inputBundle.msaaSamples);

		VkPipelineColorBlendAttachmentState colorBlendAttachment = pipelineColorBlendAttachmentState(
			VK_COLOR_COMPONENT_R_BIT |
			VK_COLOR_COMPONENT_G_BIT |
			VK_COLOR_COMPONENT_B_BIT |
			VK_COLOR_COMPONENT_A_BIT,
			VK_FALSE);
		VkPipelineColorBlendStateCreateInfo colorBlending = pipelineColorBlendStateCreateInfo(colorBlendAttachment);

		VkPipelineDepthStencilStateCreateInfo depthStencil = pipelineDepthStencilStateCreateInfo(
			VK_TRUE,
			VK_TRUE,
			VK_COMPARE_OP_LESS);

		std::vector<VkDynamicState> dynamicStates =
		{
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};
		VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = pipelineDynamicStateCreateInfo(dynamicStates);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo = pipelineLayoutCreateInfo(&inputBundle.descriptorSetLayout);

		VkPipelineLayout pipelineLayout;
		VkResult err = vkCreatePipelineLayout(inputBundle.logicalDevice, &pipelineLayoutInfo, nullptr, &pipelineLayout);
		vkUtils::check_vk_result(err, "failed to create pipeline layout!");

		VkGraphicsPipelineCreateInfo pipelineInfo = pipelineCreateInfo();
		pipelineInfo.stageCount = shaderStages.size();
		pipelineInfo.pStages = shaderStages.data();
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDepthStencilState = &depthStencil;
		pipelineInfo.pDynamicState = &dynamicStateCreateInfo;
		pipelineInfo.layout = pipelineLayout;
		pipelineInfo.renderPass = inputBundle.renderPass;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

		VkPipeline pipeline;
		err = vkCreateGraphicsPipelines(inputBundle.logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline);
		vkUtils::check_vk_result(err, "failed to create graphics pipeline!");

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