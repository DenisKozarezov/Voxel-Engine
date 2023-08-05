#include "VulkanImage.h"
#include "VulkanValidation.h"
#include "VulkanAlloc.h"

namespace vkUtils
{
	constexpr const VkFormat findSupportedFormat(
		const VkPhysicalDevice& physicalDevice, 
		const std::vector<VkFormat>& candidates, 
		const VkImageTiling& tiling, 
		const VkFormatFeatureFlags& features)
	{
		for (const VkFormat& format : candidates)
		{
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

			const bool isTilingLinear = tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features;
			const bool isTilingOptimal = tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features;

			if (isTilingLinear) return format;
			else if (isTilingOptimal) return format;
		}
		throw std::runtime_error("failed to find supported format!");
	}

	const VkFormat findDepthFormat(const VkPhysicalDevice& physicalDevice)
	{
		return findSupportedFormat(physicalDevice,
			{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
		);
	}

	constexpr bool hasStencilComponent(const VkFormat& format)
	{
		return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
	}

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
		VkDeviceMemory& imageMemory)
	{
		VkImageCreateInfo imageInfo = {};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = width;
		imageInfo.extent.height = height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = format;
		imageInfo.tiling = tiling;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = usage;
		imageInfo.samples = msaaSamples;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VkImage image;
		VkResult err = vkCreateImage(logicalDevice, &imageInfo, nullptr, &image);
		check_vk_result(err, "failed to create image!");

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(logicalDevice, image, &memRequirements);

		imageMemory = memory::allocateMemory(physicalDevice, logicalDevice, memRequirements, properties);

		vkBindImageMemory(logicalDevice, image, imageMemory, 0);
		return image;
	}

	const VkImageView createImageView(
		const VkDevice& logicalDevice,
		const VkImage& image,
		const VkFormat& format,
		const VkImageAspectFlags aspectFlags)
	{
		VkImageViewCreateInfo viewInfo = {};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = format;
		viewInfo.subresourceRange.aspectMask = aspectFlags;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		VkImageView imageView;
		VkResult err = vkCreateImageView(logicalDevice, &viewInfo, nullptr, &imageView);
		check_vk_result(err, "failed to create image view!");
		return imageView;
	}
	const VkImageView createImageView(
		const VkDevice& logicalDevice,
		const VkImage& image,
		const VkFormat& format,
		const VkComponentMapping& components,
		const VkImageAspectFlags aspectFlags)
	{
		VkImageViewCreateInfo viewInfo = {};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = format;
		viewInfo.components = components;
		viewInfo.subresourceRange.aspectMask = aspectFlags;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		VkImageView imageView;
		VkResult err = vkCreateImageView(logicalDevice, &viewInfo, nullptr, &imageView);
		check_vk_result(err, "failed to create image view!");
		return imageView;
	}
	const VkSampler createTextureSampler(const VkPhysicalDevice& physicalDevice, const VkDevice& logicalDevice)
	{
		VkPhysicalDeviceProperties properties = {};
		vkGetPhysicalDeviceProperties(physicalDevice, &properties);

		VkSamplerCreateInfo samplerInfo = {};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;

		VkSampler sampler;
		VkResult err = vkCreateSampler(logicalDevice, &samplerInfo, nullptr, &sampler);
		check_vk_result(err, "failed to create texture sampler!");
		return sampler;
	}
}