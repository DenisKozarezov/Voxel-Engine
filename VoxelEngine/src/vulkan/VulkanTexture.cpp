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
    
	void VulkanTexture::generateQuad()
    {
		createDescriptorSets();
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
		
		VOXEL_CORE_TRACE("Creating texture '{0}' [W: {1}; H: {2}; Channels: {3}]...", _filepath, _width, _height, _texChannels);

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		vulkan::createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		memory::mapMemory(_createInfo.logicalDevice, stagingBufferMemory, 0, imageSize, 0, pixels);
		stbi_image_free(pixels);

		_textureImage = memory::createImage(_createInfo.physicalDevice, _createInfo.logicalDevice, _width, _height, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _textureImageMemory);

		transitionImageLayout(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		copyBufferToImage(stagingBuffer, _textureImage);
		transitionImageLayout(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		vulkan::destroyBuffer(stagingBuffer);
		vulkan::freeDeviceMemory(stagingBufferMemory);
	}
	void VulkanTexture::createTextureImageView()
	{
		_textureImageView = memory::createImageView(_createInfo.logicalDevice, _textureImage, VK_FORMAT_R8G8B8A8_SRGB);
	}
	void VulkanTexture::createTextureSampler()
	{
		_textureSampler = memory::createTextureSampler(_createInfo.physicalDevice, _createInfo.logicalDevice);
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

		memory::destroyImageView(_createInfo.logicalDevice, _textureImageView);
		memory::destroyTextureSampler(_createInfo.logicalDevice, _textureSampler);
		memory::freeDeviceMemory(_createInfo.logicalDevice, _textureImageMemory);
		memory::destroyImage(_createInfo.logicalDevice, _textureImage);

		_textureImage = nullptr;
		_textureImageView = nullptr;
		_textureSampler = nullptr;
		_textureImageMemory = nullptr;
	}
	void VulkanTexture::copyBufferToImage(const VkBuffer& buffer, const VkImage& image)
	{
		VkCommandBuffer commandBuffer = memory::beginSingleTimeCommands();

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
		VkCommandBuffer commandBuffer = memory::beginSingleTimeCommands();

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