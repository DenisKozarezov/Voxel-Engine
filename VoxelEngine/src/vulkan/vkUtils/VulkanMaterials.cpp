#include "VulkanMaterials.h"
#include <vulkan/vkUtils/VulkanShader.h>
#include <vulkan/vkUtils/VulkanValidation.h>
#include <vulkan/vkInit/VulkanInitializers.h>
#include <assets_management/AssetsProvider.h>

namespace vkUtils
{
	std::unordered_map<string, VulkanMaterial> materials;

	void VulkanPipelineCreateInfo::build(const VkDevice& logicalDevice, const VkPipelineLayout& pipelineLayout, const VkPipelineCache& pipelineCache, VkPipeline* pipeline)
	{
		VkGraphicsPipelineCreateInfo pipelineInfo = vkInit::pipelineCreateInfo();
		pipelineInfo.flags = flags;
		pipelineInfo.pVertexInputState = vertexInputInfo;
		pipelineInfo.pInputAssemblyState = inputAssembly;
		pipelineInfo.pViewportState = viewportState;
		pipelineInfo.pRasterizationState = rasterizer;
		pipelineInfo.pMultisampleState = multisampling;
		pipelineInfo.pColorBlendState = colorBlending;
		pipelineInfo.pDepthStencilState = depthStencil;
		pipelineInfo.pDynamicState = dynamicState;
		pipelineInfo.layout = pipelineLayout;
		pipelineInfo.renderPass = *renderPass;
		pipelineInfo.subpass = 0;
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.stageCount = stagesCount;
		pipelineInfo.basePipelineHandle = basePipelineHandle;

		VkResult err = vkCreateGraphicsPipelines(logicalDevice, pipelineCache, 1, &pipelineInfo, nullptr, pipeline);
		VK_CHECK(err, "failed to create graphics pipeline!");
	}
	
	void VulkanMaterial::bind(const VkCommandBuffer& commandBuffer) const
	{
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
	}

	const VulkanMaterial* createMaterial(const VkPipeline& matPipeline, const VkPipelineLayout& matLayout, const string& matName)
	{
		vkUtils::VulkanMaterial mat;
		mat.pipeline = matPipeline;
		mat.pipelineLayout = matLayout;
		materials[matName] = mat;

		VOXEL_CORE_TRACE("Building new material '{0}'...", matName);

		return &materials[matName];
	}

	const VulkanMaterial* getMaterial(const string& matName)
	{
		VOXEL_CORE_ASSERT(materials.contains(matName), "there is no material with such name " + matName);
		return &materials[matName];
	}
	
	void makeMaterials(
		const VkDevice& logicalDevice, 
		VulkanPipelineCreateInfo& pipelineInfo,
		const VkPipelineCache& pipelineCache)
	{
		VkPipelineVertexInputStateCreateInfo vertexInputInfo = vkInit::inputStateCreateInfo({
			{ ShaderDataType::Float3 },			// Position
			{ ShaderDataType::Float3 },			// Normal
			{ ShaderDataType::Float3 },			// Color
			{ ShaderDataType::Float3, true }	// Instanced Position
		});
		pipelineInfo.vertexInputInfo = &vertexInputInfo;

		VkPipelineLayout defaultMaterialLayout;
		VkResult err = vkCreatePipelineLayout(logicalDevice, &pipelineInfo.pipelineLayoutInfo, nullptr, &defaultMaterialLayout);
		VK_CHECK(err, "failed to create pipeline layout!");

		// DEFAULT
		{
			VkPipeline defaultPipeline;
			vkUtils::VulkanShader shader = vkUtils::VulkanShader(logicalDevice, ASSET_PATH("shaders/SolidVertShader.spv"), ASSET_PATH("shaders/SolidFragShader.spv"));		
			pipelineInfo.shaderStages = shader.getStages().data();
			pipelineInfo.stagesCount = 2;
			pipelineInfo.build(logicalDevice, defaultMaterialLayout, pipelineCache, &defaultPipeline);
			createMaterial(defaultPipeline, defaultMaterialLayout, "default");
		}

		// SOLID INSTANCED
		VkPipeline solidPipeline;
		{
			vkUtils::VulkanShader shader = vkUtils::VulkanShader(logicalDevice, ASSET_PATH("shaders/InstancedSolidVert.spv"), ASSET_PATH("shaders/SolidFragShader.spv"));
			pipelineInfo.shaderStages = shader.getStages().data();
			pipelineInfo.stagesCount = 2;
			pipelineInfo.build(logicalDevice, defaultMaterialLayout, pipelineCache, &solidPipeline);
			createMaterial(solidPipeline, defaultMaterialLayout, "solid");
		}

		// NORMALS
		{
			vkUtils::VulkanShader shader = vkUtils::VulkanShader(logicalDevice, ASSET_PATH("shaders/InstancedBaseVert.spv"), ASSET_PATH("shaders/BaseFragShader.spv"), ASSET_PATH("shaders/editor/NormalGeomShader.spv"));
			pipelineInfo.shaderStages = shader.getStages().data();

			pipelineInfo.flags = VK_PIPELINE_CREATE_DERIVATIVE_BIT;
			pipelineInfo.stagesCount = 3;
			pipelineInfo.rasterizer->polygonMode = VK_POLYGON_MODE_LINE;
			pipelineInfo.inputAssembly->topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
			pipelineInfo.basePipelineHandle = solidPipeline;

			VkPipeline normals;
			pipelineInfo.build(logicalDevice, defaultMaterialLayout, pipelineCache, &normals);
			createMaterial(normals, defaultMaterialLayout, "normals");
		}

		// WIREFRAME
		VkPipeline wireframe;
		{
			vkUtils::VulkanShader shader = vkUtils::VulkanShader(logicalDevice, ASSET_PATH("shaders/InstancedSolidVert.spv"), ASSET_PATH("shaders/editor/WireframeFragShader.spv"));
			pipelineInfo.shaderStages = shader.getStages().data();

			pipelineInfo.stagesCount = 2;
			pipelineInfo.flags = VK_PIPELINE_CREATE_DERIVATIVE_BIT;
			pipelineInfo.basePipelineHandle = solidPipeline;
			pipelineInfo.rasterizer->polygonMode = VK_POLYGON_MODE_LINE;
			pipelineInfo.inputAssembly->topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			pipelineInfo.build(logicalDevice, defaultMaterialLayout, pipelineCache, &wireframe);
			createMaterial(wireframe, defaultMaterialLayout, "wireframe");
		}

		// EDITOR GRID
		vkUtils::VulkanShader shader = vkUtils::VulkanShader(logicalDevice, ASSET_PATH("shaders/editor/EditorGridVert.spv"), ASSET_PATH("shaders/editor/EditorGridFrag.spv"));
		pipelineInfo.shaderStages = shader.getStages().data();

		pipelineInfo.stagesCount = 2;
		pipelineInfo.flags = VK_PIPELINE_CREATE_DERIVATIVE_BIT;
		pipelineInfo.basePipelineHandle = wireframe;
		pipelineInfo.rasterizer->polygonMode = VK_POLYGON_MODE_FILL;
		pipelineInfo.rasterizer->cullMode = VK_CULL_MODE_FRONT_BIT;
		pipelineInfo.inputAssembly->topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		pipelineInfo.colorBlendAttachment->blendEnable = VK_TRUE;
		pipelineInfo.colorBlendAttachment->srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		pipelineInfo.colorBlendAttachment->dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		pipelineInfo.colorBlendAttachment->colorBlendOp = VK_BLEND_OP_ADD;
		pipelineInfo.colorBlendAttachment->srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		pipelineInfo.colorBlendAttachment->dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		pipelineInfo.colorBlendAttachment->alphaBlendOp = VK_BLEND_OP_ADD;

		VkPipeline editorGrid;
		pipelineInfo.build(logicalDevice, defaultMaterialLayout, pipelineCache, &editorGrid);
		createMaterial(editorGrid, defaultMaterialLayout, "editorGrid");

		VOXEL_CORE_WARN("{0} materials are successfully built.", materials.size());
	}
	
	void releaseMaterials(const VkDevice& logicalDevice)
	{
		for (auto& material : materials)
			vkDestroyPipeline(logicalDevice, material.second.pipeline, nullptr);
		materials.clear();
	}
}
