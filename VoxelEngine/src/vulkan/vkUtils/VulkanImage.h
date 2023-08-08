#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <core/PrimitiveTypes.h>

namespace vkUtils
{
	constexpr const VkFormat findSupportedFormat(
		const VkPhysicalDevice& physicalDevice, 
		const std::vector<VkFormat>& candidates, 
		const VkImageTiling& tiling, 
		const VkFormatFeatureFlags& features);

	const VkFormat findDepthFormat(const VkPhysicalDevice& physicalDevice);

	constexpr bool hasStencilComponent(const VkFormat& format);

	const VkImage createImage(
		const VkPhysicalDevice& physicalDevice,
		const VkDevice& logicalDevice,
		const uint32& width,
		const uint32& height,
		const VkFormat& format,
		const VkSampleCountFlagBits& msaaSamples,
		const VkImageTiling& tiling,
		const VkImageUsageFlags& usage,
		const VkMemoryPropertyFlags& properties,
		VkDeviceMemory& imageMemory);

	const VkImageView createImageView(
		const VkDevice& logicalDevice,
		const VkImage& image,
		const VkFormat& format,
		const VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT);

	const VkImageView createImageView(
		const VkDevice& logicalDevice,
		const VkImage& image,
		const VkFormat& format,
		const VkComponentMapping& components,
		const VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT);

	const VkSampler createTextureSampler(const VkDevice& logicalDevice, const VkSamplerCreateInfo& samplerInfo);

	const VkSampler createTextureSampler(
		const VkPhysicalDevice& physicalDevice,
		const VkDevice& logicalDevice);

	void setImageLayout(
		VkCommandBuffer cmdbuffer,
		VkImage image,
		VkImageLayout oldImageLayout,
		VkImageLayout newImageLayout,
		VkImageSubresourceRange subresourceRange,
		VkPipelineStageFlags srcStageMask,
		VkPipelineStageFlags dstStageMask);

	// Fixed sub resource on first mip level and layer
	void setImageLayout(
		VkCommandBuffer cmdbuffer,
		VkImage image,
		VkImageAspectFlags aspectMask,
		VkImageLayout oldImageLayout,
		VkImageLayout newImageLayout,
		VkPipelineStageFlags srcStageMask,
		VkPipelineStageFlags dstStageMask);
}