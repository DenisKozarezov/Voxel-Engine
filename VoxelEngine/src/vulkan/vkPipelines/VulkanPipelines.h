#pragma once
#include "../vkInit/VulkanPipeline.h"

namespace vkPipelines
{
	using Vertex = VoxelEngine::renderer::Vertex;
	
	const vkInit::GraphicsPipelineOutBundle createSolidPipeline(const vkInit::GraphicsPipilineInputBundle& inputBundle)
	{
		constexpr auto bindingDescription = vkInit::vertexInputBindingDescription(0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX);
		constexpr std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions =
		{
			vkInit::vertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Vertex::pos)),
			vkInit::vertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Vertex::normal)),
			vkInit::vertexInputAttributeDescription(0, 2, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Vertex::color))
		};

		vkUtils::VulkanShader vertexShader = vkUtils::VulkanShader(inputBundle.logicalDevice, inputBundle.vertexFilepath, VK_SHADER_STAGE_VERTEX_BIT);
		vkUtils::VulkanShader fragShader = vkUtils::VulkanShader(inputBundle.logicalDevice, inputBundle.fragmentFilepath, VK_SHADER_STAGE_FRAGMENT_BIT);

		std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages = { vertexShader.getStage(), fragShader.getStage() };

		VkPipelineVertexInputStateCreateInfo vertexInputInfo = vkInit::pipelineVertexInputStateCreateInfo(
			&bindingDescription,
			1,
			attributeDescriptions.data(),
			static_cast<uint32>(attributeDescriptions.size()));

		VkPipelineInputAssemblyStateCreateInfo inputAssembly = vkInit::pipelineInputAssemblyStateCreateInfo(
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			VK_FALSE);

		VkPipelineViewportStateCreateInfo viewportState = vkInit::pipelineViewportStateCreateInfo(1, 1);

		VkPipelineRasterizationStateCreateInfo rasterizer = vkInit::pipelineRasterizationStateCreateInfo(
			VK_POLYGON_MODE_FILL,
			VK_CULL_MODE_BACK_BIT,
			VK_FRONT_FACE_COUNTER_CLOCKWISE);

		VkPipelineMultisampleStateCreateInfo multisampling = vkInit::pipelineMultisampleStateCreateInfo(inputBundle.msaaSamples);

		VkPipelineColorBlendAttachmentState colorBlendAttachment = vkInit::pipelineColorBlendAttachmentState(
			VK_COLOR_COMPONENT_R_BIT |
			VK_COLOR_COMPONENT_G_BIT |
			VK_COLOR_COMPONENT_B_BIT |
			VK_COLOR_COMPONENT_A_BIT,
			VK_FALSE);
		VkPipelineColorBlendStateCreateInfo colorBlending = vkInit::pipelineColorBlendStateCreateInfo(colorBlendAttachment);

		VkPipelineDepthStencilStateCreateInfo depthStencil = vkInit::pipelineDepthStencilStateCreateInfo(
			VK_TRUE,
			VK_TRUE,
			VK_COMPARE_OP_LESS);

		std::vector<VkDynamicState> dynamicStates =
		{
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};
		VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = vkInit::pipelineDynamicStateCreateInfo(dynamicStates);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo = vkInit::pipelineLayoutCreateInfo(&inputBundle.descriptorSetLayout);

		vkInit::GraphicsPipelineOutBundle bundle;

		VkResult err = vkCreatePipelineLayout(inputBundle.logicalDevice, &pipelineLayoutInfo, nullptr, &bundle.layout);
		vkUtils::check_vk_result(err, "failed to create pipeline layout!");

		VkGraphicsPipelineCreateInfo pipelineInfo = vkInit::pipelineCreateInfo();
		pipelineInfo.flags = VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT;
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
		pipelineInfo.layout = bundle.layout;
		pipelineInfo.renderPass = inputBundle.renderPass;
		pipelineInfo.subpass = 0;

		err = vkCreateGraphicsPipelines(inputBundle.logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &bundle.pipeline);
		vkUtils::check_vk_result(err, "failed to create graphics pipeline!");

		vertexShader.unbind();
		fragShader.unbind();

		VOXEL_CORE_TRACE("Vulkan solid graphics pipeline created.")

		return bundle;
	}

	const vkInit::GraphicsPipelineOutBundle createNormalsPipeline(const vkInit::GraphicsPipilineInputBundle& inputBundle)
	{
		constexpr auto bindingDescription = vkInit::vertexInputBindingDescription(0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX);
		constexpr std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions =
		{
			vkInit::vertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Vertex::pos)),
			vkInit::vertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Vertex::normal)),
			vkInit::vertexInputAttributeDescription(0, 2, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Vertex::color))
		};

		vkUtils::VulkanShader vertexShader = vkUtils::VulkanShader(inputBundle.logicalDevice, inputBundle.vertexFilepath, VK_SHADER_STAGE_VERTEX_BIT);
		vkUtils::VulkanShader fragShader = vkUtils::VulkanShader(inputBundle.logicalDevice, inputBundle.fragmentFilepath, VK_SHADER_STAGE_FRAGMENT_BIT);
		vkUtils::VulkanShader geomShader = vkUtils::VulkanShader(inputBundle.logicalDevice, inputBundle.geometryFilepath, VK_SHADER_STAGE_GEOMETRY_BIT);

		std::array<VkPipelineShaderStageCreateInfo, 3> shaderStages = { vertexShader.getStage(), fragShader.getStage(), geomShader.getStage() };

		VkPipelineVertexInputStateCreateInfo vertexInputInfo = vkInit::pipelineVertexInputStateCreateInfo(
			&bindingDescription,
			1,
			attributeDescriptions.data(),
			static_cast<uint32>(attributeDescriptions.size()));

		VkPipelineInputAssemblyStateCreateInfo inputAssembly = vkInit::pipelineInputAssemblyStateCreateInfo(
			VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
			VK_FALSE);

		VkPipelineViewportStateCreateInfo viewportState = vkInit::pipelineViewportStateCreateInfo(1, 1);

		VkPipelineRasterizationStateCreateInfo rasterizer = vkInit::pipelineRasterizationStateCreateInfo(
			VK_POLYGON_MODE_LINE,
			VK_CULL_MODE_NONE,
			VK_FRONT_FACE_CLOCKWISE);

		VkPipelineDepthStencilStateCreateInfo depthStencil = vkInit::pipelineDepthStencilStateCreateInfo(
			VK_TRUE,
			VK_TRUE,
			VK_COMPARE_OP_LESS);

		std::vector<VkDynamicState> dynamicStates =
		{
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};
		VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = vkInit::pipelineDynamicStateCreateInfo(dynamicStates);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo = vkInit::pipelineLayoutCreateInfo(&inputBundle.descriptorSetLayout);

		vkInit::GraphicsPipelineOutBundle bundle;

		VkResult err = vkCreatePipelineLayout(inputBundle.logicalDevice, &pipelineLayoutInfo, nullptr, &bundle.layout);
		vkUtils::check_vk_result(err, "failed to create pipeline layout!");

		VkGraphicsPipelineCreateInfo pipelineInfo = vkInit::pipelineCreateInfo();
		pipelineInfo.flags = VK_PIPELINE_CREATE_DERIVATIVE_BIT;
		pipelineInfo.stageCount = shaderStages.size();
		pipelineInfo.pStages = shaderStages.data();
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pDepthStencilState = &depthStencil;
		pipelineInfo.pDynamicState = &dynamicStateCreateInfo;
		pipelineInfo.layout = bundle.layout;
		pipelineInfo.renderPass = inputBundle.renderPass;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = inputBundle.basePipilineHandle;

		err = vkCreateGraphicsPipelines(inputBundle.logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &bundle.pipeline);
		vkUtils::check_vk_result(err, "failed to create graphics pipeline!");

		vertexShader.unbind();
		fragShader.unbind();
		geomShader.unbind();

		VOXEL_CORE_TRACE("Vulkan normals graphics pipeline created.")

		return bundle;
	}

	const vkInit::GraphicsPipelineOutBundle createWireframePipeline(const vkInit::GraphicsPipilineInputBundle& inputBundle)
	{
		constexpr auto bindingDescription = vkInit::vertexInputBindingDescription(0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX);
		constexpr std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions =
		{
			vkInit::vertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Vertex::pos)),
			vkInit::vertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Vertex::normal)),
			vkInit::vertexInputAttributeDescription(0, 2, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Vertex::color))
		};

		vkUtils::VulkanShader vertexShader = vkUtils::VulkanShader(inputBundle.logicalDevice, inputBundle.vertexFilepath, VK_SHADER_STAGE_VERTEX_BIT);
		vkUtils::VulkanShader fragShader = vkUtils::VulkanShader(inputBundle.logicalDevice, inputBundle.fragmentFilepath, VK_SHADER_STAGE_FRAGMENT_BIT);

		std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages = { vertexShader.getStage(), fragShader.getStage() };

		VkPipelineVertexInputStateCreateInfo vertexInputInfo = vkInit::pipelineVertexInputStateCreateInfo(
			&bindingDescription,
			1,
			attributeDescriptions.data(),
			static_cast<uint32>(attributeDescriptions.size()));

		VkPipelineInputAssemblyStateCreateInfo inputAssembly = vkInit::pipelineInputAssemblyStateCreateInfo(
			VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
			VK_FALSE);

		VkPipelineViewportStateCreateInfo viewportState = vkInit::pipelineViewportStateCreateInfo(1, 1);

		VkPipelineRasterizationStateCreateInfo rasterizer = vkInit::pipelineRasterizationStateCreateInfo(
			VK_POLYGON_MODE_FILL,
			VK_CULL_MODE_NONE,
			VK_FRONT_FACE_CLOCKWISE);

		VkPipelineDepthStencilStateCreateInfo depthStencil = vkInit::pipelineDepthStencilStateCreateInfo(
			VK_TRUE,
			VK_TRUE,
			VK_COMPARE_OP_LESS);

		std::vector<VkDynamicState> dynamicStates =
		{
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};
		VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = vkInit::pipelineDynamicStateCreateInfo(dynamicStates);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo = vkInit::pipelineLayoutCreateInfo(&inputBundle.descriptorSetLayout);

		vkInit::GraphicsPipelineOutBundle bundle;

		VkResult err = vkCreatePipelineLayout(inputBundle.logicalDevice, &pipelineLayoutInfo, nullptr, &bundle.layout);
		vkUtils::check_vk_result(err, "failed to create pipeline layout!");

		VkGraphicsPipelineCreateInfo pipelineInfo = vkInit::pipelineCreateInfo();
		pipelineInfo.flags = VK_PIPELINE_CREATE_DERIVATIVE_BIT;
		pipelineInfo.stageCount = shaderStages.size();
		pipelineInfo.pStages = shaderStages.data();
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pDepthStencilState = &depthStencil;
		pipelineInfo.pDynamicState = &dynamicStateCreateInfo;
		pipelineInfo.layout = bundle.layout;
		pipelineInfo.renderPass = inputBundle.renderPass;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = inputBundle.basePipilineHandle;

		err = vkCreateGraphicsPipelines(inputBundle.logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &bundle.pipeline);
		vkUtils::check_vk_result(err, "failed to create graphics pipeline!");

		vertexShader.unbind();
		fragShader.unbind();

		VOXEL_CORE_TRACE("Vulkan wireframe graphics pipeline created.")

		return bundle;
	}
}