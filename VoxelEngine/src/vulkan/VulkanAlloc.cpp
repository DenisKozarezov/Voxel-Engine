#include "VulkanAlloc.h"
#include "VulkanValidation.h"

namespace vulkan::memory
{
	const uint32 findMemoryType(const VkPhysicalDevice& physicalDevice, const uint32& typeFilter, const VkMemoryPropertyFlags& properties)
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

		for (uint32 i = 0; i < memProperties.memoryTypeCount; i++)
		{
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			{
				return i;
			}
		}
		throw std::runtime_error("failed to find suitable memory type!");
	}
	const VkDeviceMemory allocateMemory(const VkPhysicalDevice& physicalDevice, const VkDevice& logicalDevice, const VkMemoryRequirements& requirements, const VkMemoryPropertyFlags& properties)
	{
		VkDeviceMemory memory;

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = requirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(physicalDevice, requirements.memoryTypeBits, properties);

		VkResult err = vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &memory);
		check_vk_result(err, "failed to allocate memory!");

		return memory;
	}
	const VkCommandBuffer& beginSingleTimeCommands()
	{
		VkCommandBuffer commandBuffer = CommandBuffer::allocate();
		CommandBuffer::beginCommand(commandBuffer);
		return commandBuffer;
	}
	
	void mapMemory(
		const VkDevice& logicalDevice,
		const VkDeviceMemory& deviceMemory,
		const VkDeviceSize& offset,
		const VkDeviceSize& size,
		const VkMemoryMapFlags& flags,
		const void* data)
	{
		void* dst;
		vkMapMemory(logicalDevice, deviceMemory, offset, size, flags, &dst);
		memcpy(dst, data, size);
		vkUnmapMemory(logicalDevice, deviceMemory);
	}
	
	void createBuffer(const VkPhysicalDevice& physicalDevice, const VkDevice& logicalDevice, const VkDeviceSize& size, const VkBufferUsageFlags& usage, const VkMemoryPropertyFlags& properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
	{
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VkResult err = vkCreateBuffer(logicalDevice, &bufferInfo, nullptr, &buffer);
		check_vk_result(err, "failed to create buffer!");

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(logicalDevice, buffer, &memRequirements);

		bufferMemory = allocateMemory(physicalDevice, logicalDevice, memRequirements, properties);

		vkBindBufferMemory(logicalDevice, buffer, bufferMemory, 0);
	}
	void destroyBuffer(const VkDevice& logicalDevice, const VkBuffer& buffer)
	{
		vkDestroyBuffer(logicalDevice, buffer, nullptr);
	}
	void freeDeviceMemory(const VkDevice& logicalDevice, const VkDeviceMemory& memory)
	{
		vkFreeMemory(logicalDevice, memory, nullptr);
	}

	const VkImage createImage(
		const VkPhysicalDevice& physicalDevice,
		const VkDevice& logicalDevice,
		const uint32& width,
		const uint32& height,
		const VkFormat& format,
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
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VkImage image;
		VkResult err = vkCreateImage(logicalDevice, &imageInfo, nullptr, &image);
		check_vk_result(err, "failed to create image!");

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(logicalDevice, image, &memRequirements);

		imageMemory = allocateMemory(physicalDevice, logicalDevice, memRequirements, properties);

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
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
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

	void destroyImage(const VkDevice& logicalDevice, const VkImage& image)
	{
		vkDestroyImage(logicalDevice, image, nullptr);
	}
	void destroyImageView(const VkDevice& logicalDevice, const VkImageView& imageView)
	{
		vkDestroyImageView(logicalDevice, imageView, nullptr);
	}
	void destroyTextureSampler(const VkDevice& logicalDevice, const VkSampler& sampler)
	{
		vkDestroySampler(logicalDevice, sampler, nullptr);
	}
}