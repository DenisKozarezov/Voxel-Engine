#include "VulkanFramebuffer.h"
#include "VulkanRenderer.h"

namespace VoxelEngine::renderer
{
	Framebuffer::Framebuffer(const VkDevice& logicalDevice, const VkRenderPass& renderPass, const std::vector<VkImageView>& attachments, const VkExtent2D& framebufferSize)
		: _logicalDevice(logicalDevice)
	{		
		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = static_cast<uint32>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = framebufferSize.width;
		framebufferInfo.height = framebufferSize.height;
		framebufferInfo.layers = 1;

		VkResult err = vkCreateFramebuffer(logicalDevice, &framebufferInfo, nullptr, &_framebuffer);
		check_vk_result(err, "failed to create framebuffer!");
	}
	void Framebuffer::release() const
	{
		vkDestroyFramebuffer(_logicalDevice, _framebuffer, nullptr);
	}
}