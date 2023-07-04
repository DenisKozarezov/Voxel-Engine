#pragma once
#include <vulkan/vulkan.h>
#include <vector>

namespace VoxelEngine::renderer
{
	class Framebuffer
	{
	private:
		VkFramebuffer _framebuffer;
		VkDevice _logicalDevice;
	public:
		Framebuffer() = default;
		Framebuffer(const VkDevice& logicalDevice, const VkRenderPass& renderPass, const std::vector<VkImageView>& attachments, const VkExtent2D& framebufferSize);

		operator VkFramebuffer() const & { return _framebuffer; }

		void release() const;

		~Framebuffer() = default;
	};
}