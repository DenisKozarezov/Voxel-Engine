#include "VulkanMaterials.h"
#include <vulkan/vkUtils/VulkanShader.h>
#include <vulkan/vkUtils/VulkanValidation.h>
#include <vulkan/vkInit/VulkanInitializers.h>
#include <vulkan/VulkanBackend.h>

namespace vkUtils
{
	MaterialsCache materials = MaterialsCache(MATERIALS_MAX);
	
	void VulkanMaterial::bind() const
	{
		const auto& frame = vulkan::getCurrentFrame();
		bind(frame.commandBuffer, frame.descriptorSet);
	}
	void VulkanMaterial::bind(const VkCommandBuffer& commandBuffer, const VkDescriptorSet& descriptorSet) const
	{
		vkCmdBindDescriptorSets(
			commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			shaderPass.pipelineLayout,
			0,
			1,
			&descriptorSet,
			0,
			nullptr);
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shaderPass.pipeline);
	}

	void createMaterial(
		const VkDevice& logicalDevice,
		const vkInit::ShaderPass shaderPass,
		const string& matName,
		const bool& instanced)
	{
		materials[matName] = MakeShared<const VulkanMaterial>(logicalDevice, shaderPass, instanced);
		RUNTIME_TRACE("Building new material '{0}'...", matName);
	}

	void makeMaterials(const VkDevice& device, vkInit::VulkanGraphicsPipelineBuilder& pipelineBuilder)
	{
		// DEFAULT
		VkPipeline defaultPipeline;
		VulkanShader defaultShader = VulkanShader(device, Paths::shaderWorkingDir() + "default_shader.glsl");
		{			
			VkPipelineVertexInputStateCreateInfo vertexInputInfo = vkInit::inputStateCreateInfo({
				{ ShaderDataType::Float3_S32 },			// Position
				{ ShaderDataType::Float3_S32 }			// Color
			}, 24);
						
			pipelineBuilder
				.setVertexInputState(&vertexInputInfo)
				.setStages(defaultShader.getStages());

			auto shaderPass = pipelineBuilder.build();
			defaultPipeline = shaderPass.pipeline;
			createMaterial(device, shaderPass, "default");
		}

		// LINES
		{
			constexpr VkPipelineRasterizationStateCreateInfo rasterizer = vkInit::pipelineRasterizationStateCreateInfo(
				VK_POLYGON_MODE_LINE,
				VK_CULL_MODE_BACK_BIT,
				VK_FRONT_FACE_COUNTER_CLOCKWISE);

			constexpr VkPipelineInputAssemblyStateCreateInfo inputAssembly = vkInit::pipelineInputAssemblyStateCreateInfo(
				VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
				VK_FALSE);
			
			pipelineBuilder
				.setFlags(VK_PIPELINE_CREATE_DERIVATIVE_BIT)
				.setBasePipelineHandle(defaultPipeline)
				.setRasterizationState(&rasterizer)
				.setInputAssemblyState(&inputAssembly)
				.setStages(defaultShader.getStages());
			
			auto shaderPass = pipelineBuilder.build();
			createMaterial(device, shaderPass, "lines");
		}

		// SOLID
		VkPipeline solidPipeline;
		{
			VulkanShader shader = VulkanShader(device, Paths::shaderWorkingDir() + "solid_shader.glsl");
			
			VkPipelineVertexInputStateCreateInfo vertexInputInfo = vkInit::inputStateCreateInfo({
				{ ShaderDataType::Float3_S32 },			// Position
				{ ShaderDataType::Float3_S32 },			// Normal
				{ ShaderDataType::Float3_S32 },			// Color
			});

			constexpr VkPipelineRasterizationStateCreateInfo rasterizer = vkInit::pipelineRasterizationStateCreateInfo(
				VK_POLYGON_MODE_FILL,
				VK_CULL_MODE_BACK_BIT,
				VK_FRONT_FACE_COUNTER_CLOCKWISE);

			constexpr VkPipelineInputAssemblyStateCreateInfo inputAssembly = vkInit::pipelineInputAssemblyStateCreateInfo(
				VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
				VK_FALSE);
			
			pipelineBuilder
				.setFlags(VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT)
				.setVertexInputState(&vertexInputInfo)
				.setStages(shader.getStages())
				.setRasterizationState(&rasterizer)
				.setInputAssemblyState(&inputAssembly);
	
			auto shaderPass = pipelineBuilder.build();
			solidPipeline = shaderPass.pipeline;
			createMaterial(device, shaderPass, "solid");
		}

		// SOLID INSTANCED
		VkPipeline solidInstancedPipeline;
		{
			VulkanShader shader = VulkanShader(device, Paths::shaderWorkingDir() + "solid_instanced_shader.glsl");
			
			VkPipelineVertexInputStateCreateInfo vertexInputInfo = vkInit::inputStateCreateInfo({
				{ ShaderDataType::Float3_S32 },				// Position
				{ ShaderDataType::Float3_S32 },				// Normal
				{ ShaderDataType::Float3_S32 },				// Color
				{ ShaderDataType::Float3_S32, true }	// Instanced Position
			});
			
			pipelineBuilder
				.setFlags(VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT)
				.setVertexInputState(&vertexInputInfo)
				.setBasePipelineHandle(solidPipeline)
				.setStages(shader.getStages());
			
			auto shaderPass = pipelineBuilder.build();
			solidInstancedPipeline = shaderPass.pipeline;
			createMaterial(device, shaderPass, "solid_instanced", true);
		}

		// NORMALS COLOR
		VkPipeline normals;
		{
			VulkanShader shader = VulkanShader(device, Paths::shaderWorkingDir() + "normals_color_shader.glsl");
			
			VkPipelineVertexInputStateCreateInfo vertexInputInfo = vkInit::inputStateCreateInfo({
				{ ShaderDataType::Float3_S32 },			// Position
				{ ShaderDataType::Float3_S32 },			// Normal
			});
			
			pipelineBuilder
				.setFlags(VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT)
				.setVertexInputState(&vertexInputInfo)
				.setBasePipelineHandle(solidPipeline)
				.setStages(shader.getStages());
			
			auto shaderPass = pipelineBuilder.build();
			normals = shaderPass.pipeline;
			createMaterial(device, shaderPass, "normals");
		}

		// NORMALS COLOR INSTANCED
		{
			VulkanShader shader = VulkanShader(device, Paths::shaderWorkingDir() + "normals_color_instanced_shader.glsl");
			
			VkPipelineVertexInputStateCreateInfo vertexInputInfo = vkInit::inputStateCreateInfo({
				{ ShaderDataType::Float3_S32 },				// Position
				{ ShaderDataType::Float3_S32 },				// Normal,
				{ ShaderDataType::Float3_S32, true },	// Instanced Position
			});

			pipelineBuilder
				.setFlags(VK_PIPELINE_CREATE_DERIVATIVE_BIT)
				.setVertexInputState(&vertexInputInfo)
				.setBasePipelineHandle(normals)
				.setStages(shader.getStages());
			
			auto shaderPass = pipelineBuilder.build();
			createMaterial(device, shaderPass, "normals_instanced", true);
		}

		// NORMALS LINES
		VkPipeline normalsLines;
		{
			VulkanShader shader = VulkanShader(device, Paths::shaderWorkingDir() + "normals_lines_shader.glsl");
			
			VkPipelineVertexInputStateCreateInfo vertexInputInfo = vkInit::inputStateCreateInfo({
				{ ShaderDataType::Float3_S32 },			// Position
				{ ShaderDataType::Float3_S32 },			// Normal
				  { ShaderDataType::Float3_S32 },			// Color
			});

			constexpr VkPipelineRasterizationStateCreateInfo rasterizer = vkInit::pipelineRasterizationStateCreateInfo(
				VK_POLYGON_MODE_LINE,
				VK_CULL_MODE_BACK_BIT,
				VK_FRONT_FACE_COUNTER_CLOCKWISE);

			constexpr VkPipelineInputAssemblyStateCreateInfo inputAssembly = vkInit::pipelineInputAssemblyStateCreateInfo(
				VK_PRIMITIVE_TOPOLOGY_LINE_STRIP,
				VK_FALSE);
			
			pipelineBuilder
				.setFlags(VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT)
				.setVertexInputState(&vertexInputInfo)
				.setRasterizationState(&rasterizer)
				.setInputAssemblyState(&inputAssembly)
				.setBasePipelineHandle(normals)
				.setStages(shader.getStages());
			
			auto shaderPass = pipelineBuilder.build();
			normalsLines = shaderPass.pipeline;
			createMaterial(device, shaderPass, "normals_lines");
		}

		// NORMALS LINES INSTANCED
		{
			VulkanShader shader = VulkanShader(device, Paths::shaderWorkingDir() + "normals_lines_instanced_shader.glsl");
			
			VkPipelineVertexInputStateCreateInfo vertexInputInfo = vkInit::inputStateCreateInfo({
				{ ShaderDataType::Float3_S32 },				// Position
				{ ShaderDataType::Float3_S32 },				// Normal
				{ ShaderDataType::Float3_S32 },				// Color
				{ ShaderDataType::Float3_S32, true }	// Instanced Position
			});
			
			pipelineBuilder
				.setFlags(VK_PIPELINE_CREATE_DERIVATIVE_BIT)
				.setVertexInputState(&vertexInputInfo)
				.setBasePipelineHandle(normalsLines)
				.setStages(shader.getStages());

			auto shaderPass = pipelineBuilder.build();
			createMaterial(device, shaderPass, "normals_lines_instanced");
		}

		// WIREFRAME
		VkPipeline wireframe;
		{
			VulkanShader shader = VulkanShader(device, Paths::shaderWorkingDir() + "wireframe_shader.glsl");
			
			VkPipelineVertexInputStateCreateInfo vertexInputInfo = vkInit::inputStateCreateInfo({
				{ ShaderDataType::Float3_S32 },			// Position
			});

			constexpr VkPipelineRasterizationStateCreateInfo rasterizer = vkInit::pipelineRasterizationStateCreateInfo(
				VK_POLYGON_MODE_LINE,
				VK_CULL_MODE_BACK_BIT,
				VK_FRONT_FACE_COUNTER_CLOCKWISE);

			constexpr VkPipelineInputAssemblyStateCreateInfo inputAssembly = vkInit::pipelineInputAssemblyStateCreateInfo(
				VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
				VK_FALSE);
			
			pipelineBuilder
				.setFlags(VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT)
				.setVertexInputState(&vertexInputInfo)
				.setRasterizationState(&rasterizer)
				.setInputAssemblyState(&inputAssembly)
				.setBasePipelineHandle(solidInstancedPipeline)
				.setStages(shader.getStages());

			auto shaderPass = pipelineBuilder.build();
			wireframe = shaderPass.pipeline;
			createMaterial(device, shaderPass, "wireframe");
		}

		// WIREFRAME INSTANCED
		{
			VulkanShader shader = VulkanShader(device, Paths::shaderWorkingDir() + "wireframe_instanced_shader.glsl");
			
			VkPipelineVertexInputStateCreateInfo vertexInputInfo = vkInit::inputStateCreateInfo({
				{ ShaderDataType::Float3_S32 },				// Position
				{ ShaderDataType::Float3_S32, true }	// Instanced Position
			});

			pipelineBuilder
				.setFlags(VK_PIPELINE_CREATE_DERIVATIVE_BIT)
				.setVertexInputState(&vertexInputInfo)
				.setBasePipelineHandle(wireframe)
				.setStages(shader.getStages());

			auto shaderPass = pipelineBuilder.build();
			createMaterial(device, shaderPass, "wireframe_instanced", true);
		}

		// EDITOR GRID
		{
			VulkanShader shader = VulkanShader(device, Paths::shaderWorkingDir() + "editor/editor_grid_shader.glsl");
			
			VkPipelineVertexInputStateCreateInfo vertexInputInfo = vkInit::inputStateCreateInfo({
				{ ShaderDataType::Float3_S32 },			// Position
				{ ShaderDataType::Float3_S32 }			// Normal
			});

			constexpr VkPipelineRasterizationStateCreateInfo rasterizer = vkInit::pipelineRasterizationStateCreateInfo(
				VK_POLYGON_MODE_FILL,
				VK_CULL_MODE_NONE,
				VK_FRONT_FACE_COUNTER_CLOCKWISE);

			constexpr VkPipelineDepthStencilStateCreateInfo depthStencil = vkInit::pipelineDepthStencilStateCreateInfo(
				VK_TRUE,
				VK_FALSE,
				VK_COMPARE_OP_LESS);

			VkPipelineColorBlendAttachmentState colorBlendAttachment = vkInit::pipelineColorBlendAttachmentState(
				VK_COLOR_COMPONENT_R_BIT |
				VK_COLOR_COMPONENT_G_BIT |
				VK_COLOR_COMPONENT_B_BIT |
				VK_COLOR_COMPONENT_A_BIT,
				VK_TRUE);
			colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
			colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
			colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
			colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
			
			pipelineBuilder
				.setFlags(VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT)
				.setVertexInputState(&vertexInputInfo)
				.setRasterizationState(&rasterizer)
				.setDepthStencilState(&depthStencil)
				.setColorBlendAttachment(&colorBlendAttachment)
				.setStages(shader.getStages());
						
			auto shaderPass = pipelineBuilder.build();
			createMaterial(device, shaderPass, "editor_grid");
		}
		
		RUNTIME_WARN("{0} materials are successfully built.", materials.size());
	}
	
	void releaseMaterials()
	{
		for (auto &material : materials)
		{
			EDITOR_TRACE("Unregistering material with name '{0}'...", material.first);
			material.second = nullptr;			
		}
		materials.clear();
	}
}

namespace utils
{
	const TSharedPtr<const VoxelEngine::renderer::IMaterial>& getMaterial(const string& matName)
	{
		RUNTIME_ASSERT(vkUtils::materials.contains(matName), "there is no material with such name " + matName);
		return vkUtils::materials[matName];
	}
}