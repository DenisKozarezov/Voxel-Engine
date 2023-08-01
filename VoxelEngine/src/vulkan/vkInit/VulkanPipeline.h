#pragma once
#include "VulkanInitializers.h"
#include "../vkUtils/VulkanValidation.h"
#include "../vkUtils/VulkanShader.h"
#include <core/renderer/Vertex.h>

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
		string geometryFilepath;
		VkPipeline basePipilineHandle;
	};

	typedef struct Pipelines
	{
		VkPipeline solid;
		VkPipeline wireframe;
		VkPipeline normals;

		void release(const VkDevice& logicalDevice) const
		{
			vkDestroyPipeline(logicalDevice, solid, nullptr);
			vkDestroyPipeline(logicalDevice, wireframe, nullptr);
			vkDestroyPipeline(logicalDevice, normals, nullptr);
		}
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
}