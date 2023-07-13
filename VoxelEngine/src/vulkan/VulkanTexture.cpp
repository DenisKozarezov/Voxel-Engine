#include "VulkanTexture.h"
#include "VulkanBackend.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace vulkan
{
	VulkanTexture::VulkanTexture(const std::string& path, const TextureCreateInfo& createInfo) 
		: Texture(path),
		_createInfo(createInfo)
	{
		createTextureImage(path);
		createTextureImageView();
		createTextureSampler();
		generateQuad();
	}
    
	const VkImageView VulkanTexture::createImageView(const VkFormat& format) const
	{
		VkImageViewCreateInfo viewInfo = {};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = _textureImage;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = format;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		VkImageView imageView;
		VkResult err = vkCreateImageView(_createInfo.logicalDevice, &viewInfo, nullptr, &imageView);
		check_vk_result(err, "failed to create texture image view!");
		return imageView;
	}
	void VulkanTexture::generateQuad()
    {
		Texture::generateQuad();
		createDescriptorSets();
	}
    void VulkanTexture::createImage(const uint32& width, const uint32& height, const VkFormat& format, const VkImageTiling& tiling, const VkImageUsageFlags& usage, const VkMemoryPropertyFlags& properties, VkImage& image, VkDeviceMemory& imageMemory)
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
		
		VkResult err = vkCreateImage(_createInfo.logicalDevice, &imageInfo, nullptr, &image);
		check_vk_result(err, "failed to create image!");

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(_createInfo.logicalDevice, image, &memRequirements);

		_textureImageMemory = vulkan::allocateMemory(memRequirements, properties);

		vkBindImageMemory(_createInfo.logicalDevice, _textureImage, _textureImageMemory, 0);
	}
	void VulkanTexture::createTextureImage(const string& filepath)
	{
		int width, height, texChannels;

		stbi_uc* pixels = stbi_load(filepath.c_str(), &width, &height, &texChannels, STBI_rgb_alpha);
		VkDeviceSize imageSize = width * height * 4;

		_width = static_cast<uint32>(width);
		_height = static_cast<uint32>(height);
		_texChannels = static_cast<uint16>(texChannels);

		VOXEL_CORE_ASSERT(pixels, "failed to load texture image on path '" + filepath + "'")

		std::stringstream ss;
		ss << "Creating texture '" << filepath << "' [W: " << _width << "; H: " << _height << "; Channels: " << _texChannels << "]...";
		VOXEL_CORE_TRACE(ss.str())

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		vulkan::createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		recordToBuffer(pixels, imageSize, stagingBufferMemory);
		createImage(_width, _height, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _textureImage, _textureImageMemory);

		transitionImageLayout(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		copyBufferToImage(stagingBuffer, _textureImage);
		transitionImageLayout(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		vulkan::destroyBuffer(stagingBuffer);
		vulkan::freeDeviceMemory(stagingBufferMemory);
	}
	void VulkanTexture::createTextureImageView()
	{
		_textureImageView = createImageView(VK_FORMAT_R8G8B8A8_SRGB);
	}
	void VulkanTexture::createTextureSampler()
	{
		VkPhysicalDeviceProperties properties = {};
		vkGetPhysicalDeviceProperties(_createInfo.physicalDevice, &properties);

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

		VkResult err = vkCreateSampler(_createInfo.logicalDevice, &samplerInfo, nullptr, &_textureSampler);
		check_vk_result(err, "failed to create texture sampler!");
	}
	void VulkanTexture::createDescriptorSets()
	{
		std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, _createInfo.descriptorSetLayout);
		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = _createInfo.descriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
		allocInfo.pSetLayouts = layouts.data();

		_descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
		VkResult err = vkAllocateDescriptorSets(_createInfo.logicalDevice, &allocInfo, _descriptorSets.data());
		check_vk_result(err, "failed to allocate descriptor sets!");

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
		{
			VkDescriptorBufferInfo bufferInfo = {};
			auto uniformBuffer = dynamic_cast<VulkanUniformBuffer*>(_uniformBuffers[i]);
			bufferInfo.buffer = uniformBuffer->operator VkBuffer();
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(VoxelEngine::renderer::UniformBufferObject);

			VkDescriptorImageInfo imageInfo = {};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = _textureImageView;
			imageInfo.sampler = _textureSampler;

			std::array<VkWriteDescriptorSet, 2> descriptorWrites = {};

			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = _descriptorSets[i];
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &bufferInfo;

			descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[1].dstSet = _descriptorSets[i];
			descriptorWrites[1].dstBinding = 1;
			descriptorWrites[1].dstArrayElement = 0;
			descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[1].descriptorCount = 1;
			descriptorWrites[1].pImageInfo = &imageInfo;

			vkUpdateDescriptorSets(_createInfo.logicalDevice, static_cast<uint32>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		}
	}
	void VulkanTexture::setUniformBuffer(const void* ubo, const size_t& size)
	{
		_uniformBuffers[vulkan::CURRENT_FRAME]->setData(ubo, size);
	}
	void VulkanTexture::render()
	{	
		Texture::render();

		VkCommandBuffer commandBuffer = vulkan::getCommandBuffer();
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _createInfo.pipelineLayout, 0, 1, &_descriptorSets[vulkan::CURRENT_FRAME], 0, nullptr);
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32>(_indices.size()), 1, 0, 0, 0);
	}
	void VulkanTexture::release()
	{
		Texture::release();

		vkDestroyImageView(_createInfo.logicalDevice, _textureImageView, nullptr);
		vkDestroySampler(_createInfo.logicalDevice, _textureSampler, nullptr);
		vulkan::freeDeviceMemory(_textureImageMemory);
		vkDestroyImage(_createInfo.logicalDevice, _textureImage, nullptr);

		_textureImage = nullptr;
		_textureImageView = nullptr;
		_textureSampler = nullptr;
		_textureImageMemory = nullptr;
	}
	void VulkanTexture::recordToBuffer(void* src, const VkDeviceSize& imageSize, const VkDeviceMemory& stagingBufferMemory) const
	{
		void* data;
		vkMapMemory(_createInfo.logicalDevice, stagingBufferMemory, 0, imageSize, 0, &data);
		memcpy(data, src, static_cast<size_t>(imageSize));
		vkUnmapMemory(_createInfo.logicalDevice, stagingBufferMemory);
		stbi_image_free(src);
	}
	void VulkanTexture::copyBufferToImage(const VkBuffer& buffer, const VkImage& image)
	{
		VkCommandBuffer commandBuffer = vulkan::beginSingleTimeCommands();

		VkBufferImageCopy region = {};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;
		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = {
			static_cast<uint32>(_width),
			static_cast<uint32>(_height),
			1
		};
		vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
		
		vulkan::endSingleTimeCommands(commandBuffer);
	}
	void VulkanTexture::transitionImageLayout(const VkFormat& format, const VkImageLayout& oldLayout, const VkImageLayout& newLayout) const
	{
		VkCommandBuffer commandBuffer = vulkan::beginSingleTimeCommands();

		VkImageMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = _textureImage;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;

		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else
		{
			throw std::invalid_argument("unsupported layout transition!");
		}

		vkCmdPipelineBarrier(
			commandBuffer,
			sourceStage, destinationStage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);

		vulkan::endSingleTimeCommands(commandBuffer);
	}
	
	VulkanTexture::~VulkanTexture()
	{
		release();
	}
}