#include "VulkanMaterials.h"
#include <vulkan/vkUtils/VulkanShader.h>
#include <vulkan/vkUtils/VulkanValidation.h>
#include <vulkan/vkInit/VulkanInitializers.h>
#include <vulkan/VulkanBackend.h>

namespace vkUtils
{
	MaterialsCache materials = MaterialsCache(MATERIALS_MAX);

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
	
	void VulkanMaterial::bind() const
	{
		const auto& frame = vulkan::getCurrentFrame();
		bind(frame.commandBuffer, frame.descriptorSet);
	}
	void VulkanMaterial::bind(const VkCommandBuffer& commandBuffer, const VkDescriptorSet& descriptorSet) const
	{
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
	}

	const VulkanMaterial* createMaterial(
		const VkDevice& logicalDevice,
		const VkPipeline& matPipeline,
		const VkPipelineLayout& matLayout,
		const string& matName,
		const bool& instanced)
	{
		VulkanMaterial* mat = new VulkanMaterial{instanced};
		mat->logicalDevice = logicalDevice;
		mat->pipeline = matPipeline;
		mat->pipelineLayout = matLayout;
		materials[matName] = mat;

		VOXEL_CORE_TRACE("Building new material '{0}'...", matName);

		return materials[matName];
	}

	void unregisterMaterial(const string& matName)
	{
		if (auto* mat = getMaterial(matName))
		{
			VOXEL_TRACE("Unregistering material with name '{0}'...", matName);
			delete mat;
			materials.erase(matName);
		}
	}

	const VulkanMaterial* getMaterial(const string& matName)
	{
		VOXEL_CORE_ASSERT(materials.contains(matName), "there is no material with such name " + matName);
		return materials[matName];
	}

	void makeMaterials(
		const vkInit::VulkanDevice& device, 
		const VkPipelineCache& pipelineCache,
		VulkanPipelineCreateInfo& pipelineInfo)
	{
		// DEFAULT
		VkPipeline defaultPipeline;
		{
			VkPipelineVertexInputStateCreateInfo vertexInputInfo = vkInit::inputStateCreateInfo({
				{ ShaderDataType::Float3_S32 },			// Position
				{ ShaderDataType::Float3_S32 }			// Color
			}, 24);
			pipelineInfo.vertexInputInfo = &vertexInputInfo;

			VkPipelineLayout defaultMaterialLayout;
			VkResult err = vkCreatePipelineLayout(device.logicalDevice, &pipelineInfo.pipelineLayoutInfo, nullptr, &defaultMaterialLayout);
			VK_CHECK(err, "failed to create pipeline layout!");

			VulkanShader shader = VulkanShader(device.logicalDevice, SHADERS_PATH("default_shader.glsl"));
			pipelineInfo.shaderStages = shader.getStages().data();
			pipelineInfo.stagesCount = static_cast<uint32>(shader.getStages().size());
			pipelineInfo.rasterizer->polygonMode = VK_POLYGON_MODE_FILL;
			pipelineInfo.inputAssembly->topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			pipelineInfo.build(device.logicalDevice, defaultMaterialLayout, pipelineCache, &defaultPipeline);
			createMaterial(device.logicalDevice, defaultPipeline, defaultMaterialLayout, "default");
		}

		// LINES
		{
			VkPipelineLayout linesMaterialLayout;
			VkResult err = vkCreatePipelineLayout(device.logicalDevice, &pipelineInfo.pipelineLayoutInfo, nullptr, &linesMaterialLayout);
			VK_CHECK(err, "failed to create pipeline layout!");

			VkPipeline linesPipeline;
			VulkanShader shader = VulkanShader(device.logicalDevice, SHADERS_PATH("default_shader.glsl"));
			pipelineInfo.shaderStages = shader.getStages().data();
			pipelineInfo.stagesCount = static_cast<uint32>(shader.getStages().size());
			pipelineInfo.rasterizer->polygonMode = VK_POLYGON_MODE_LINE;
			pipelineInfo.inputAssembly->topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
			pipelineInfo.flags = VK_PIPELINE_CREATE_DERIVATIVE_BIT;
			pipelineInfo.basePipelineHandle = defaultPipeline;
			pipelineInfo.build(device.logicalDevice, linesMaterialLayout, pipelineCache, &linesPipeline);
			createMaterial(device.logicalDevice, linesPipeline, linesMaterialLayout, "lines");
		}

		// SOLID
		VkPipeline solidPipeline;
		{
			VkPipelineVertexInputStateCreateInfo vertexInputInfo = vkInit::inputStateCreateInfo({
				{ ShaderDataType::Float3_S32 },			// Position
				{ ShaderDataType::Float3_S32 },			// Normal
				{ ShaderDataType::Float3_S32 },			// Color
			});
			pipelineInfo.vertexInputInfo = &vertexInputInfo;

			VkPipelineLayout solidMaterialLayout;
			VkResult err = vkCreatePipelineLayout(device.logicalDevice, &pipelineInfo.pipelineLayoutInfo, nullptr, &solidMaterialLayout);
			VK_CHECK(err, "failed to create pipeline layout!");

			VulkanShader shader = VulkanShader(device.logicalDevice, SHADERS_PATH("solid_shader.glsl"));
			pipelineInfo.shaderStages = shader.getStages().data();
			pipelineInfo.stagesCount = static_cast<uint32>(shader.getStages().size());
			pipelineInfo.rasterizer->polygonMode = VK_POLYGON_MODE_FILL;
			pipelineInfo.inputAssembly->topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			pipelineInfo.flags = VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT;
			pipelineInfo.build(device.logicalDevice, solidMaterialLayout, pipelineCache, &solidPipeline);
			createMaterial(device.logicalDevice, solidPipeline, solidMaterialLayout, "solid");
		}

		// SOLID INSTANCED
		VkPipeline solidInstancedPipeline;
		{
			VkPipelineVertexInputStateCreateInfo vertexInputInfo = vkInit::inputStateCreateInfo({
				{ ShaderDataType::Float3_S32 },			// Position
				{ ShaderDataType::Float3_S32 },			// Normal
				{ ShaderDataType::Float3_S32 },			// Color
				{ ShaderDataType::Float3_S32, true }	// Instanced Position
			});
			pipelineInfo.vertexInputInfo = &vertexInputInfo;

			VkPipelineLayout solidMaterialLayout;
			VkResult err = vkCreatePipelineLayout(device.logicalDevice, &pipelineInfo.pipelineLayoutInfo, nullptr, &solidMaterialLayout);
			VK_CHECK(err, "failed to create pipeline layout!");
			
			VulkanShader shader = VulkanShader(device.logicalDevice, SHADERS_PATH("solid_instanced_shader.glsl"));
			pipelineInfo.shaderStages = shader.getStages().data();
			pipelineInfo.stagesCount = static_cast<uint32>(shader.getStages().size());
			pipelineInfo.rasterizer->polygonMode = VK_POLYGON_MODE_FILL;
			pipelineInfo.inputAssembly->topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			pipelineInfo.flags = VK_PIPELINE_CREATE_DERIVATIVE_BIT;
			pipelineInfo.basePipelineHandle = solidPipeline;
			pipelineInfo.build(device.logicalDevice, solidMaterialLayout, pipelineCache, &solidInstancedPipeline);
			createMaterial(device.logicalDevice, solidInstancedPipeline, solidMaterialLayout, "solid_instanced", true);
		}

		// NORMALS COLOR
		VkPipeline normals;
		{
			VkPipelineVertexInputStateCreateInfo vertexInputInfo = vkInit::inputStateCreateInfo({
				{ ShaderDataType::Float3_S32 },			// Position
				{ ShaderDataType::Float3_S32 },			// Normal
			});
			pipelineInfo.vertexInputInfo = &vertexInputInfo;

			VkPipelineLayout normalsMaterialLayout;
			VkResult err = vkCreatePipelineLayout(device.logicalDevice, &pipelineInfo.pipelineLayoutInfo, nullptr, &normalsMaterialLayout);
			VK_CHECK(err, "failed to create pipeline layout!");

			VulkanShader shader = VulkanShader(device.logicalDevice, SHADERS_PATH("normals_color_shader.glsl"));
			pipelineInfo.shaderStages = shader.getStages().data();
			pipelineInfo.stagesCount = static_cast<uint32>(shader.getStages().size());
			pipelineInfo.flags = VK_PIPELINE_CREATE_DERIVATIVE_BIT;
			pipelineInfo.rasterizer->polygonMode = VK_POLYGON_MODE_FILL;
			pipelineInfo.inputAssembly->topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			pipelineInfo.basePipelineHandle = solidPipeline;

			pipelineInfo.build(device.logicalDevice, normalsMaterialLayout, pipelineCache, &normals);
			createMaterial(device.logicalDevice, normals, normalsMaterialLayout, "normals");
		}

		// NORMALS INSTANCED
		{
			VkPipelineVertexInputStateCreateInfo vertexInputInfo = vkInit::inputStateCreateInfo({
				{ ShaderDataType::Float3_S32 },			// Position
				{ ShaderDataType::Float3_S32 },			// Normal,
				{ ShaderDataType::Float3_S32, true },	// Instanced Position
			});
			pipelineInfo.vertexInputInfo = &vertexInputInfo;

			VkPipelineLayout normalsMaterialLayout;
			VkResult err = vkCreatePipelineLayout(device.logicalDevice, &pipelineInfo.pipelineLayoutInfo, nullptr, &normalsMaterialLayout);
			VK_CHECK(err, "failed to create pipeline layout!");

			VulkanShader shader = VulkanShader(device.logicalDevice, SHADERS_PATH("normals_color_instanced_shader.glsl"));
			pipelineInfo.shaderStages = shader.getStages().data();
			pipelineInfo.stagesCount = static_cast<uint32>(shader.getStages().size());
			pipelineInfo.flags = VK_PIPELINE_CREATE_DERIVATIVE_BIT;
			pipelineInfo.basePipelineHandle = normals;

			VkPipeline normalsInstanced;
			pipelineInfo.build(device.logicalDevice, normalsMaterialLayout, pipelineCache, &normalsInstanced);
			createMaterial(device.logicalDevice, normalsInstanced, normalsMaterialLayout, "normals_instanced", true);
		}

		// NORMALS LINES
		VkPipeline normalsLines;
		{
			VkPipelineVertexInputStateCreateInfo vertexInputInfo = vkInit::inputStateCreateInfo({
				{ ShaderDataType::Float3_S32 },			// Position
				{ ShaderDataType::Float3_S32 },			// Normal
				});
			pipelineInfo.vertexInputInfo = &vertexInputInfo;

			VkPipelineLayout normalsMaterialLayout;
			VkResult err = vkCreatePipelineLayout(device.logicalDevice, &pipelineInfo.pipelineLayoutInfo, nullptr, &normalsMaterialLayout);
			VK_CHECK(err, "failed to create pipeline layout!");

			VulkanShader shader = VulkanShader(device.logicalDevice, SHADERS_PATH("normals_lines_shader.glsl"));
			pipelineInfo.shaderStages = shader.getStages().data();
			pipelineInfo.stagesCount = static_cast<uint32>(shader.getStages().size());
			pipelineInfo.flags = VK_PIPELINE_CREATE_DERIVATIVE_BIT;
			pipelineInfo.rasterizer->polygonMode = VK_POLYGON_MODE_LINE;
			pipelineInfo.inputAssembly->topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
			pipelineInfo.basePipelineHandle = normals;

			pipelineInfo.build(device.logicalDevice, normalsMaterialLayout, pipelineCache, &normalsLines);
			createMaterial(device.logicalDevice, normalsLines, normalsMaterialLayout, "normals_lines");
		}

		// NORMALS LINES INSTANCED
		{
			VkPipeline normalsLinesInstanced;
			VkPipelineVertexInputStateCreateInfo vertexInputInfo = vkInit::inputStateCreateInfo({
				{ ShaderDataType::Float3_S32 },			// Position
				{ ShaderDataType::Float3_S32 },			// Normal
				});
			pipelineInfo.vertexInputInfo = &vertexInputInfo;

			VkPipelineLayout normalsMaterialLayout;
			VkResult err = vkCreatePipelineLayout(device.logicalDevice, &pipelineInfo.pipelineLayoutInfo, nullptr, &normalsMaterialLayout);
			VK_CHECK(err, "failed to create pipeline layout!");

			VulkanShader shader = VulkanShader(device.logicalDevice, SHADERS_PATH("normals_lines_instanced_shader.glsl"));
			pipelineInfo.shaderStages = shader.getStages().data();
			pipelineInfo.stagesCount = static_cast<uint32>(shader.getStages().size());
			pipelineInfo.flags = VK_PIPELINE_CREATE_DERIVATIVE_BIT;
			pipelineInfo.rasterizer->polygonMode = VK_POLYGON_MODE_LINE;
			pipelineInfo.inputAssembly->topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
			pipelineInfo.basePipelineHandle = normalsLines;

			pipelineInfo.build(device.logicalDevice, normalsMaterialLayout, pipelineCache, &normalsLinesInstanced);
			createMaterial(device.logicalDevice, normalsLinesInstanced, normalsMaterialLayout, "normals_lines_instanced");
		}

		// WIREFRAME
		VkPipeline wireframe;
		{
			VkPipelineVertexInputStateCreateInfo vertexInputInfo = vkInit::inputStateCreateInfo({
				{ ShaderDataType::Float3_S32 },			// Position
			});
			pipelineInfo.vertexInputInfo = &vertexInputInfo;

			VkPipelineLayout wireframeMaterialLayout;
			VkResult err = vkCreatePipelineLayout(device.logicalDevice, &pipelineInfo.pipelineLayoutInfo, nullptr, &wireframeMaterialLayout);
			VK_CHECK(err, "failed to create pipeline layout!");

			VulkanShader shader = VulkanShader(device.logicalDevice, SHADERS_PATH("wireframe_shader.glsl"));
			pipelineInfo.shaderStages = shader.getStages().data();
			pipelineInfo.stagesCount = static_cast<uint32>(shader.getStages().size());
			pipelineInfo.flags = VK_PIPELINE_CREATE_DERIVATIVE_BIT;
			pipelineInfo.rasterizer->polygonMode = VK_POLYGON_MODE_LINE;
			pipelineInfo.inputAssembly->topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			pipelineInfo.basePipelineHandle = solidInstancedPipeline;
			pipelineInfo.build(device.logicalDevice, wireframeMaterialLayout, pipelineCache, &wireframe);
			createMaterial(device.logicalDevice, wireframe, wireframeMaterialLayout, "wireframe");
		}

		// WIREFRAME INSTANCED
		{
			VkPipeline wireframeInstanced;
			VkPipelineVertexInputStateCreateInfo vertexInputInfo = vkInit::inputStateCreateInfo({
				{ ShaderDataType::Float3_S32 },			// Position
				{ ShaderDataType::Float3_S32, true}		// Instanced Position
			});
			pipelineInfo.vertexInputInfo = &vertexInputInfo;

			VkPipelineLayout wireframeMaterialLayout;
			VkResult err = vkCreatePipelineLayout(device.logicalDevice, &pipelineInfo.pipelineLayoutInfo, nullptr, &wireframeMaterialLayout);
			VK_CHECK(err, "failed to create pipeline layout!");

			VulkanShader shader = VulkanShader(device.logicalDevice, SHADERS_PATH("wireframe_instanced_shader.glsl"));
			pipelineInfo.shaderStages = shader.getStages().data();
			pipelineInfo.stagesCount = static_cast<uint32>(shader.getStages().size());
			pipelineInfo.basePipelineHandle = wireframe;
			pipelineInfo.build(device.logicalDevice, wireframeMaterialLayout, pipelineCache, &wireframeInstanced);
			createMaterial(device.logicalDevice, wireframeInstanced, wireframeMaterialLayout, "wireframe_instanced", true);
		}

		// EDITOR GRID
		{
			VkPipelineVertexInputStateCreateInfo vertexInputInfo = vkInit::inputStateCreateInfo({
				{ ShaderDataType::Float3_S32 },			// Position
				{ ShaderDataType::Float3_S32 },			// Normal
				{ ShaderDataType::Float3_S32 },			// Color
			});
			pipelineInfo.vertexInputInfo = &vertexInputInfo;

			VkPipelineLayout editorGridMaterialLayout;
			VkResult err = vkCreatePipelineLayout(device.logicalDevice, &pipelineInfo.pipelineLayoutInfo, nullptr, &editorGridMaterialLayout);
			VK_CHECK(err, "failed to create pipeline layout!");

			VulkanShader shader = VulkanShader(device.logicalDevice, SHADERS_PATH("editor/editor_grid_shader.glsl"));
			pipelineInfo.shaderStages = shader.getStages().data();
			pipelineInfo.stagesCount = static_cast<uint32>(shader.getStages().size());
			pipelineInfo.flags = VK_PIPELINE_CREATE_DERIVATIVE_BIT;
			pipelineInfo.rasterizer->polygonMode = VK_POLYGON_MODE_FILL;
			pipelineInfo.rasterizer->cullMode = VK_CULL_MODE_NONE;
			pipelineInfo.basePipelineHandle = wireframe;
			pipelineInfo.depthStencil->depthTestEnable = VK_TRUE;
			pipelineInfo.depthStencil->depthWriteEnable = false;
			pipelineInfo.depthStencil->depthCompareOp = VK_COMPARE_OP_LESS;
			pipelineInfo.colorBlendAttachment->blendEnable = VK_TRUE;
			pipelineInfo.colorBlendAttachment->srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
			pipelineInfo.colorBlendAttachment->dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			pipelineInfo.colorBlendAttachment->colorBlendOp = VK_BLEND_OP_ADD;
			pipelineInfo.colorBlendAttachment->srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
			pipelineInfo.colorBlendAttachment->dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			pipelineInfo.colorBlendAttachment->alphaBlendOp = VK_BLEND_OP_ADD;

			VkPipeline editorGrid;
			pipelineInfo.build(device.logicalDevice, editorGridMaterialLayout, pipelineCache, &editorGrid);
			createMaterial(device.logicalDevice, editorGrid, editorGridMaterialLayout, "editor_grid");
		}

		// RAYMARCH QUAD
		{
			VkPipelineVertexInputStateCreateInfo emptyInputState = vkInit::emptyInputStateCreateInfo();
			pipelineInfo.vertexInputInfo = &emptyInputState;

			VkPipelineLayout fullscreenQuadMaterialLayout;
			VkResult err = vkCreatePipelineLayout(device.logicalDevice, &pipelineInfo.pipelineLayoutInfo, nullptr, &fullscreenQuadMaterialLayout);
			VK_CHECK(err, "failed to create pipeline layout!");

			VulkanShader shader = VulkanShader(device.logicalDevice, SHADERS_PATH("ray_marching/ray_marching_shader.glsl"));
			pipelineInfo.shaderStages = shader.getStages().data();
			pipelineInfo.stagesCount = static_cast<uint32>(shader.getStages().size());
			pipelineInfo.flags = VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT;
			pipelineInfo.rasterizer->polygonMode = VK_POLYGON_MODE_FILL;
			pipelineInfo.rasterizer->cullMode = VK_CULL_MODE_FRONT_BIT;
			pipelineInfo.rasterizer->frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
			pipelineInfo.inputAssembly->topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

			VkPipeline raymarchQuad;
			pipelineInfo.build(device.logicalDevice, fullscreenQuadMaterialLayout, pipelineCache, &raymarchQuad);
			createMaterial(device.logicalDevice, raymarchQuad, fullscreenQuadMaterialLayout, "raymarch_quad");
		}
		VOXEL_CORE_WARN("{0} materials are successfully built.", materials.size());
	}
	
	void releaseMaterials()
	{
		while (!materials.empty())
		{
			const auto& matName = materials.begin()->first;
			unregisterMaterial(matName);
		}
	}
}

namespace utils
{
	const VoxelEngine::components::mesh::IMaterial* getMaterial(const string& matName)
	{
		return vkUtils::getMaterial(matName);
	}
}