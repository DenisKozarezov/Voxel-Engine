//#include "VulkanImage.h"
//#include "VulkanRenderer.h"
//
//#define STB_IMAGE_IMPLEMENTATION
//#include <stb_image/stb_image.h>
//
//namespace VoxelEngine::renderer
//{
//	const void VulkanImage::createImage(const VkFormat& format, const VkImageTiling& tiling, const VkImageUsageFlags& usage, const VkMemoryPropertyFlags& properties)
//	{
//		VkImageCreateInfo imageInfo = {};
//		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
//		imageInfo.imageType = VK_IMAGE_TYPE_2D;
//		imageInfo.extent.width = _width;
//		imageInfo.extent.height = _height;
//		imageInfo.extent.depth = 1;
//		imageInfo.mipLevels = 1;
//		imageInfo.arrayLayers = 1;
//		imageInfo.format = format;
//		imageInfo.tiling = tiling;
//		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//		imageInfo.usage = usage;
//		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
//		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
//
//		if (vkCreateImage(_logicalDevice, &imageInfo, nullptr, &_image) != VK_SUCCESS)
//		{
//			throw std::runtime_error("failed to create image!");
//		}
//
//		VkMemoryRequirements memRequirements;
//		vkGetImageMemoryRequirements(_logicalDevice, _image, &memRequirements);
//
//		VkMemoryAllocateInfo allocInfo = {};
//		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
//		allocInfo.allocationSize = memRequirements.size;
//		allocInfo.memoryTypeIndex = VulkanRenderer::findMemoryType(memRequirements.memoryTypeBits, properties);
//		
//		if (vkAllocateMemory(_logicalDevice, &allocInfo, nullptr, &_memory) != VK_SUCCESS)
//		{
//			throw std::runtime_error("failed to allocate image memory!");
//		}
//		vkBindImageMemory(_logicalDevice, _image, _memory, 0);
//	}
//	const void VulkanImage::createImageView(const VkFormat& format, const VkImageAspectFlags& aspectFlags)
//	{
//		VkImageViewCreateInfo viewInfo = {};
//		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
//		viewInfo.image = _image;
//		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
//		viewInfo.format = format;
//		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//		viewInfo.subresourceRange.baseMipLevel = 0;
//		viewInfo.subresourceRange.levelCount = 1;
//		viewInfo.subresourceRange.baseArrayLayer = 0;
//		viewInfo.subresourceRange.layerCount = 1;
//		viewInfo.subresourceRange.aspectMask = aspectFlags;
//
//		if (vkCreateImageView(_logicalDevice, &viewInfo, nullptr, &_imageView) != VK_SUCCESS)
//		{
//			throw std::runtime_error("failed to create texture image view!");
//		}
//	}
//	const void VulkanImage::createTextureSampler()
//	{
//		VkPhysicalDeviceProperties properties = {};
//		vkGetPhysicalDeviceProperties(_physicalDevice, &properties);
//
//		VkSamplerCreateInfo samplerInfo = {};
//		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
//		samplerInfo.magFilter = VK_FILTER_LINEAR;
//		samplerInfo.minFilter = VK_FILTER_LINEAR;
//		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
//		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
//		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
//		samplerInfo.anisotropyEnable = VK_TRUE;
//		samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
//		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
//		samplerInfo.unnormalizedCoordinates = VK_FALSE;
//		samplerInfo.compareEnable = VK_FALSE;
//		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
//		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
//		samplerInfo.mipLodBias = 0.0f;
//		samplerInfo.minLod = 0.0f;
//
//		if (vkCreateSampler(_logicalDevice, &samplerInfo, nullptr, &_sampler) != VK_SUCCESS)
//		{
//			throw std::runtime_error("failed to create texture sampler!");
//		}
//	}
//	const void VulkanImage::createBuffer(const VkDeviceSize& size, const VkBufferUsageFlags& usage, const VkMemoryPropertyFlags& properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) const
//	{
//		VkBufferCreateInfo bufferInfo = {};
//		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
//		bufferInfo.size = size;
//		bufferInfo.usage = usage;
//		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
//
//		if (vkCreateBuffer(_logicalDevice, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
//		{
//			throw std::runtime_error("failed to create buffer!");
//		}
//
//		VkMemoryRequirements memRequirements;
//		vkGetBufferMemoryRequirements(_logicalDevice, buffer, &memRequirements);
//
//		VkMemoryAllocateInfo allocInfo = {};
//		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
//		allocInfo.allocationSize = memRequirements.size;
//		allocInfo.memoryTypeIndex = VulkanRenderer::findMemoryType(memRequirements.memoryTypeBits, properties);
//
//		if (vkAllocateMemory(_logicalDevice, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
//		{
//			throw std::runtime_error("failed to allocate buffer memory!");
//		}
//
//		vkBindBufferMemory(_logicalDevice, buffer, bufferMemory, 0);
//	}
//	void VulkanImage::allocate()
//	{
//		createImage(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
//		createImageView(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
//		createTextureSampler();
//		//_descriptorSet = (VkDescriptorSet)ImGui_ImplVulkan_AddTexture(_sampler, _imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
//	}
//	void VulkanImage::release()
//	{
//		vkDestroyImage(_logicalDevice, _image, nullptr);
//		vkDestroySampler(_logicalDevice, _sampler, nullptr);
//		vkDestroyImageView(_logicalDevice, _imageView, nullptr);
//		vkFreeMemory(_logicalDevice, _memory, nullptr);
//		vkDestroyBuffer(_logicalDevice, _stagingBuffer, nullptr);
//		vkFreeMemory(_logicalDevice, _stagingBufferMemory, nullptr);
//
//		_sampler = nullptr;
//		_imageView = nullptr;
//		_image = nullptr;
//		_memory = nullptr;
//		_stagingBuffer = nullptr;
//		_stagingBufferMemory = nullptr;
//	}
//	void VulkanImage::setData(void* data)
//	{
//		const size_t upload_size = _width * _height * 4;
//
//		if (!_stagingBuffer)
//		{
//			createBuffer(upload_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, _stagingBuffer, _stagingBufferMemory);
//		}
//		recordToBuffer(data, upload_size);
//		transitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
//		
//	}
//	void VulkanImage::resize(const uint32& width, const uint32& height)
//	{
//		if (_image && _width == width && _height == height)
//			return;
//
//		// TODO: max size?
//
//		_width = width;
//		_height = height;
//
//		release();
//		allocate();
//	}
//	const void VulkanImage::recordToBuffer(void* src, const VkDeviceSize& imageSize) const
//	{
//		void* data;
//		vkMapMemory(_logicalDevice, _stagingBufferMemory, 0, imageSize, 0, &data);
//		memcpy(data, src, static_cast<size_t>(imageSize));
//		vkUnmapMemory(_logicalDevice, _stagingBufferMemory);
//		stbi_image_free(src);
//	}
//	const void VulkanImage::transitionImageLayout(const VkImageLayout& oldLayout, const VkImageLayout& newLayout) const
//	{
//		VkCommandBuffer commandBuffer = VulkanRenderer::beginSingleTimeCommands();
//
//		VkImageMemoryBarrier barrier = {};
//		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
//		barrier.oldLayout = oldLayout;
//		barrier.newLayout = newLayout;
//		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//		barrier.image = _image;
//		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//		barrier.subresourceRange.baseMipLevel = 0;
//		barrier.subresourceRange.levelCount = 1;
//		barrier.subresourceRange.baseArrayLayer = 0;
//		barrier.subresourceRange.layerCount = 1;
//
//		VkPipelineStageFlags sourceStage;
//		VkPipelineStageFlags destinationStage;
//
//		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
//		{
//			barrier.srcAccessMask = 0;
//			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//
//			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
//			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
//		}
//		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
//		{
//			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
//
//			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
//			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
//		}
//		else
//		{
//			throw std::invalid_argument("unsupported layout transition!");
//		}
//
//		vkCmdPipelineBarrier(
//			commandBuffer,
//			sourceStage, destinationStage,
//			0,
//			0, nullptr,
//			0, nullptr,
//			1, &barrier
//		);
//
//		VulkanRenderer::endSingleTimeCommands(commandBuffer);
//	}
//	VulkanImage::VulkanImage(const std::string_view& path) : Image(path)
//	{
//		_logicalDevice = VulkanRenderer::getLogicalDevice();
//		_physicalDevice = VulkanRenderer::getPhysicalDevice();
//
//		int width, height, channels;
//		uint8* data = nullptr;
//
//		if (stbi_is_hdr(path.data()))
//		{
//			data = (uint8*)stbi_loadf(path.data(), &width, &height, &channels, 4);
//			_format = ImageFormat::RGBA32F;
//		}
//		else
//		{
//			data = stbi_load(path.data(), &width, &height, &channels, 4);
//			_format = ImageFormat::RGBA;
//		}
//
//		_width = width;
//		_height = height;
//
//		allocate();
//		if (data) setData(data);
//	}
//	VulkanImage::VulkanImage(const uint32& width, const uint32& height, const ImageFormat& format, void* data)
//		: Image(width, height, format)
//	{
//		_logicalDevice = VulkanRenderer::getLogicalDevice();
//		_physicalDevice = VulkanRenderer::getPhysicalDevice();
//
//		allocate();
//		if (data) setData(data);
//	}
//
//	VulkanImage::~VulkanImage()
//	{
//		release();
//	}
//}