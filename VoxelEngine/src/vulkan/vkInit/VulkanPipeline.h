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
		VkPipelineCache pipelineCache;
		VkDescriptorSetLayout descriptorSetLayout;
		VkSampleCountFlagBits msaaSamples;
		string vertexFilepath;
		string fragmentFilepath;
		string geometryFilepath;
		VkPipeline basePipilineHandle;
	};

	struct Pipelines
	{
		VkPipeline solid;
		VkPipeline wireframe;
		VkPipeline normals;
		VkPipeline editorGrid;
		VkPipeline viewport;

		void release(const VkDevice& logicalDevice) const
		{
			vkDestroyPipeline(logicalDevice, solid, nullptr);
			vkDestroyPipeline(logicalDevice, wireframe, nullptr);
			vkDestroyPipeline(logicalDevice, normals, nullptr);
			vkDestroyPipeline(logicalDevice, editorGrid, nullptr);
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

		// Subpass dependencies for layout transitions
		VkSubpassDependency dependency1;
		dependency1.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency1.dstSubpass = 0;
		dependency1.srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		dependency1.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		dependency1.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		dependency1.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
		dependency1.dependencyFlags = 0;

		VkSubpassDependency dependency2;
		dependency2.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency2.dstSubpass = 0;
		dependency2.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency2.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency2.srcAccessMask = 0;
		dependency2.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
		dependency2.dependencyFlags = 0;

		std::vector<VkAttachmentDescription> attachments = { colorAttachment, depthAttachment, colorAttachment };
		std::vector<VkSubpassDescription> subpasses = { subpass };
		std::vector<VkSubpassDependency> dependencies = { dependency1, dependency2 };
		VkRenderPassCreateInfo renderPassInfo = renderPassCreateInfo(attachments, subpasses, dependencies);

		VkRenderPass renderPass;
		VkResult err = vkCreateRenderPass(logicalDevice, &renderPassInfo, nullptr, &renderPass);
		vkUtils::check_vk_result(err, "failed to create render pass!");

		VOXEL_CORE_TRACE("Vulkan render pass created.")

		return renderPass;
	}

	const VkRenderPass createViewportRenderPass(
		const VkDevice& logicalDevice,
		const VkFormat& swapChainImageFormat,
		const VkFormat depthFormat,
		const VkSampleCountFlagBits& msaaSamples)
	{
		VkAttachmentDescription colorAttachment = renderPassColorAttachment(swapChainImageFormat, msaaSamples);
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentDescription depthAttachment = renderPassDepthAttachment(depthFormat, msaaSamples);

		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentRef{};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		subpass.pDepthStencilAttachment = &depthAttachmentRef;
		subpass.inputAttachmentCount = 0;
		subpass.pInputAttachments = nullptr;
		subpass.preserveAttachmentCount = 0;
		subpass.pPreserveAttachments = nullptr;
		subpass.pResolveAttachments = nullptr;

		VkSubpassDependency dependency1;
		dependency1.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency1.dstSubpass = 0;
		dependency1.srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		dependency1.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency1.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		dependency1.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependency1.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		VkSubpassDependency dependency2;
		dependency2.srcSubpass = 0;
		dependency2.dstSubpass = VK_SUBPASS_EXTERNAL;
		dependency2.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency2.dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		dependency2.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependency2.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		dependency2.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		std::vector<VkAttachmentDescription> attachments = { colorAttachment, depthAttachment };
		std::vector<VkSubpassDescription> subpasses = { subpass };
		std::vector<VkSubpassDependency> dependencies = { dependency1, dependency2 };
		VkRenderPassCreateInfo renderPassInfo = renderPassCreateInfo(attachments, subpasses, dependencies);

		VkRenderPass renderPass;
		VkResult err = vkCreateRenderPass(logicalDevice, &renderPassInfo, nullptr, &renderPass);
		vkUtils::check_vk_result(err, "failed to create render pass!");

		VOXEL_CORE_TRACE("Vulkan viewport render pass created.")

		return renderPass;
	}
}