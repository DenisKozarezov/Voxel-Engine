#pragma once
#include "VulkanSwapchain.h"

namespace vkInit
{
	VkFramebuffer createFramebuffer(
		const VulkanDevice& device,
		const VkRenderPass& renderPass,
		const VkExtent2D& swapChainExtent,
		const std::vector<VkImageView>& attachments)
	{
		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = static_cast<uint32>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = swapChainExtent.width;
		framebufferInfo.height = swapChainExtent.height;
		framebufferInfo.layers = 1;

		VkFramebuffer framebuffer;
		VkResult err = vkCreateFramebuffer(device.logicalDevice, &framebufferInfo, nullptr, &framebuffer);
		VK_CHECK(err, "failed to create framebuffer!");

		VOXEL_CORE_TRACE("Vulkan framebuffer allocated.");

		return framebuffer;
	}
}