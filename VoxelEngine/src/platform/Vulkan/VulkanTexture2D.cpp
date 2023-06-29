#include "VulkanTexture2D.h"
#include "VulkanRenderer.h"
#include <imgui_impl_vulkan.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace VoxelEngine::renderer
{
	SharedRef<VulkanRenderer> _renderer = 0;

	VulkanTexture2D::VulkanTexture2D(const std::string& path, VkAllocationCallbacks* allocator) 
		: Texture2D(path),
		_allocator(allocator)
	{
		_renderer = VulkanRenderer::getInstance();
		_logicalDevice = _renderer->getLogicalDevice();

		createTextureImage(path);
		createTextureImageView();
		createTextureSampler();
		generateQuad();
		createDescriptorSetLayout();
		createDescriptorSet();
	}
    
	const VkImageView VulkanTexture2D::createImageView(const VkFormat& format) const
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
		if (vkCreateImageView(_logicalDevice, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create texture image view!");
		}
		return imageView;
	}
	void VulkanTexture2D::generateQuad()
    {
		_vertexBuffer = VertexBuffer(_vertices.data(), sizeof(_vertices[0]) * _vertices.size(), _allocator);
		_indexBuffer = IndexBuffer(_indices.data(), sizeof(_indices[0]) * _indices.size(), _allocator);
		
		VkDeviceSize bufferSize = sizeof(UniformBufferObject);
		_uniformBuffer = UniformBuffer(bufferSize, _allocator);
	}
    void VulkanTexture2D::createImage(const VkFormat& format, const VkImageTiling& tiling, const VkImageUsageFlags& usage, const VkMemoryPropertyFlags& properties)
	{
		VkImageCreateInfo imageInfo = {};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = _width;
		imageInfo.extent.height = _height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = format;
		imageInfo.tiling = tiling;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = usage;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateImage(_logicalDevice, &imageInfo, nullptr, &_textureImage) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create image!");
		}

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(_logicalDevice, _textureImage, &memRequirements);

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = VulkanRenderer::getInstance()->findMemoryType(memRequirements.memoryTypeBits, properties);
		
		if (vkAllocateMemory(_logicalDevice, &allocInfo, _allocator, &_stagingMemory) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate image memory!");
		}
		vkBindImageMemory(_logicalDevice, _textureImage, _stagingMemory, 0);
	}
	void VulkanTexture2D::createTextureImage(const string& filepath)
	{
		stbi_uc* pixels = stbi_load(filepath.c_str(), &_width, &_height, &_texChannels, STBI_rgb_alpha);
		VkDeviceSize imageSize = _width * _height * 4;

		if (!pixels) {
			throw std::runtime_error("failed to load texture image on path " + filepath);
		}

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		_renderer->createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		recordToBuffer(pixels, imageSize, stagingBufferMemory);
		createImage(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		transitionImageLayout(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		copyBufferToImage(stagingBuffer, _textureImage);
		transitionImageLayout(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		vkDestroyBuffer(_logicalDevice, stagingBuffer, _allocator);
		vkFreeMemory(_logicalDevice, stagingBufferMemory, _allocator);
	}
	void VulkanTexture2D::createTextureImageView()
	{
		_textureImageView = createImageView(VK_FORMAT_R8G8B8A8_SRGB);
	}
	void VulkanTexture2D::createTextureSampler()
	{
		VkPhysicalDeviceProperties properties = {};
		auto physicalDevice = _renderer->getPhysicalDevice();
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

		if (vkCreateSampler(_logicalDevice, &samplerInfo, _allocator, &_textureSampler) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create texture sampler!");
		}
	}
	void VulkanTexture2D::createDescriptorSet()
	{
		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = VulkanRenderer::getInstance()->_descriptorPool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &_descriptorSetLayout;

		if (vkAllocateDescriptorSets(_logicalDevice, &allocInfo, &_descriptorSet) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate descriptor sets!");
		}

		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = _uniformBuffer;
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferObject);

		VkDescriptorImageInfo imageInfo = {};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = _textureImageView;
		imageInfo.sampler = _textureSampler;

		std::array<VkWriteDescriptorSet, 2> descriptorWrites = {};

		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = _descriptorSet;
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &bufferInfo;

		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstSet = _descriptorSet;
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pImageInfo = &imageInfo;

		vkUpdateDescriptorSets(_logicalDevice, static_cast<uint32>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}
	void VulkanTexture2D::createDescriptorSetLayout()
	{
		VkDescriptorSetLayoutBinding uboLayoutBinding = {};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.pImmutableSamplers = nullptr;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

		VkDescriptorSetLayoutBinding samplerLayoutBinding{};
		samplerLayoutBinding.binding = 1;
		samplerLayoutBinding.descriptorCount = 1;
		samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		samplerLayoutBinding.pImmutableSamplers = nullptr;
		samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };
		VkDescriptorSetLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32>(bindings.size());
		layoutInfo.pBindings = bindings.data();

		VkResult err = vkCreateDescriptorSetLayout(_logicalDevice, &layoutInfo, _allocator, &_descriptorSetLayout);
		VulkanRenderer::check_vk_result(err, "failed to create descriptor set layout!");
	}
	void VulkanTexture2D::bind()
	{
		UniformBufferObject ubo = {};
		ubo.model = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.projection = glm::perspective(glm::radians(45.0f), 3.0f / 2.0f, 0.1f, 10.0f);
		ubo.projection[1][1] *= -1;

		_uniformBuffer.setData(&ubo);				
		_vertexBuffer.bind();
		_indexBuffer.bind();

		VkCommandBuffer commandBuffer = _renderer->getCommandBuffer();
		//vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _renderer->_pipelineLayout, 0, 1, &_descriptorSet, 0, nullptr);
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32>(_indices.size()), 1, 0, 0, 0);
	}
	void VulkanTexture2D::release()
	{
		vkDestroyImageView(_logicalDevice, _textureImageView, _allocator);
		vkDestroySampler(_logicalDevice, _textureSampler, _allocator);
		vkFreeMemory(_logicalDevice, _stagingMemory, _allocator);
		vkDestroyImage(_logicalDevice, _textureImage, _allocator);
		vkDestroyDescriptorSetLayout(_logicalDevice, _descriptorSetLayout, _allocator);

		_indexBuffer.release();
		_vertexBuffer.release();
		_uniformBuffer.release();

		_textureSampler = nullptr;
		_textureImageView = nullptr;
		_textureImage = nullptr;
		_stagingMemory = nullptr;
	}
	void VulkanTexture2D::recordToBuffer(void* src, const VkDeviceSize& imageSize, const VkDeviceMemory& stagingBufferMemory) const
	{
		void* data;
		vkMapMemory(_logicalDevice, stagingBufferMemory, 0, imageSize, 0, &data);
		memcpy(data, src, static_cast<size_t>(imageSize));
		vkUnmapMemory(_logicalDevice, stagingBufferMemory);
		stbi_image_free(src);
	}
	void VulkanTexture2D::copyBufferToImage(const VkBuffer& buffer, const VkImage& image)
	{
		VkCommandBuffer commandBuffer = CommandBuffer::allocate();
		CommandBuffer::beginCommand(commandBuffer);

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
		
		CommandBuffer::endCommand(commandBuffer);
	}
	void VulkanTexture2D::transitionImageLayout(const VkFormat& format, const VkImageLayout& oldLayout, const VkImageLayout& newLayout) const
	{
		VkCommandBuffer commandBuffer = CommandBuffer::allocate();
		CommandBuffer::beginCommand(commandBuffer);

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

		CommandBuffer::endCommand(commandBuffer);
	}
	
	VulkanTexture2D::~VulkanTexture2D()
	{
		release();
	}
}