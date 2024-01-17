#pragma once
#include <vulkan/vkInit/VulkanDevice.h>

namespace vkUtils
{
	constexpr VkFormat findSupportedFormat(
		const VkPhysicalDevice& physicalDevice, 
		const std::vector<VkFormat>& candidates, 
		const VkImageTiling& tiling, 
		const VkFormatFeatureFlags& features);

	VkFormat findDepthFormat(const VkPhysicalDevice& physicalDevice);

	constexpr bool hasStencilComponent(const VkFormat& format);

	VkImage createImage(
		const vkInit::VulkanDevice* device,
		const uint32& width,
		const uint32& height,
		const VkFormat& format,
		const VkSampleCountFlagBits& msaaSamples,
		const VkImageTiling& tiling,
		const VkImageUsageFlags& usage,
		const VkMemoryPropertyFlags& properties,
		VkDeviceMemory& imageMemory);

	VkImageView createImageView(
		const VkDevice& logicalDevice,
		const VkImage& image,
		const VkFormat& format,
		const VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT);

	VkImageView createImageView(
		const VkDevice& logicalDevice,
		const VkImage& image,
		const VkFormat& format,
		const VkComponentMapping& components,
		const VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT);

	VkSampler createTextureSampler(const vkInit::VulkanDevice& device, const VkSamplerCreateInfo& samplerInfo);

	VkSampler createTextureSampler(const vkInit::VulkanDevice& device);

	void setImageLayout(
		VkCommandBuffer cmdbuffer,
		VkImage image,
		VkImageLayout oldImageLayout,
		VkImageLayout newImageLayout,
		VkImageSubresourceRange subresourceRange,
		VkPipelineStageFlags srcStageMask,
		VkPipelineStageFlags dstStageMask);

	void setImageLayout(
		VkCommandBuffer cmdbuffer,
		VkImage image,
		VkImageAspectFlags aspectMask,
		VkImageLayout oldImageLayout,
		VkImageLayout newImageLayout,
		VkPipelineStageFlags srcStageMask,
		VkPipelineStageFlags dstStageMask);
}