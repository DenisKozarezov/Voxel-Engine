#pragma once
#include <vector>
#include <vulkan/vulkan.h>
#include <Core/CoreTypes.h>

namespace vkInit
{
	struct VulkanDevice;
}

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
		VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT);

	VkImageView createImageView(
		const VkDevice& logicalDevice,
		const VkImage& image,
		const VkFormat& format,
		const VkComponentMapping& components,
		VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT);

	VkSampler createTextureSampler(const vkInit::VulkanDevice& device, const VkSamplerCreateInfo& samplerInfo);

	VkSampler createTextureSampler(const vkInit::VulkanDevice& device);

	void setImageLayout(
		const VkCommandBuffer& cmdbuffer,
		const VkImage& image,
		const VkImageLayout& oldImageLayout,
		const VkImageLayout& newImageLayout,
		const VkImageSubresourceRange& subresourceRange,
		const VkPipelineStageFlags& srcStageMask,
		const VkPipelineStageFlags& dstStageMask);

	void setImageLayout(
		const VkCommandBuffer& cmdbuffer,
		const VkImage& image,
		const VkImageAspectFlags& aspectMask,
		const VkImageLayout& oldImageLayout,
		const VkImageLayout& newImageLayout,
		const VkPipelineStageFlags& srcStageMask,
		const VkPipelineStageFlags& dstStageMask);
}
