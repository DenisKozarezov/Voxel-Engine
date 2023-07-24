#pragma once
#include "VulkanSwapchain.h"

namespace vulkan
{
	void createFramebuffers(
		const VkDevice& logicalDevice,
		const VkRenderPass& renderPass,
		const VkExtent2D& swapChainExtent,
		std::vector<SwapChainFrame>& frames) 
	{
		for (int i = 0; i < frames.size(); ++i) {

			std::vector<VkImageView> attachments =
			{
				frames[i].imageView
			};
			
			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = renderPass;
			framebufferInfo.attachmentCount = static_cast<uint32>(attachments.size());
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = swapChainExtent.width;
			framebufferInfo.height = swapChainExtent.height;
			framebufferInfo.layers = 1;

			VkResult err = vkCreateFramebuffer(logicalDevice, &framebufferInfo, nullptr, &frames[i].framebuffer);
			check_vk_result(err, "failed to create framebuffer!");
		}
	}

	void destroyFramebuffer(const VkDevice& logicalDevice, const VkFramebuffer& framebuffer)
	{
		vkDestroyFramebuffer(logicalDevice, framebuffer, nullptr);
	}
}