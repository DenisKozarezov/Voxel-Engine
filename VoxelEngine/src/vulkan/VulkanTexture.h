#pragma once
#include <vulkan/vulkan.h>
#include <core/Log.h>

namespace vulkan
{
	struct TextureCreateInfo
	{
		VkDevice logicalDevice;
		VkPhysicalDevice physicalDevice;
		VkPipelineLayout pipelineLayout;
		VkDescriptorSetLayout descriptorSetLayout;
		VkDescriptorPool descriptorPool;
		VkExtent2D swapChainExtent;
	};

	class VulkanTexture
	{
	private:
		TextureCreateInfo _createInfo;

		VkImage _textureImage;
		VkDeviceMemory _textureImageMemory;
		VkImageView _textureImageView;
		VkSampler _textureSampler;
		std::vector<VkDescriptorSet> _descriptorSets;

		void generateQuad();
		void createTextureImage(const string& filepath);
		void createTextureImageView();
		void createTextureSampler();
		void createDescriptorSets();
		void copyBufferToImage(const VkBuffer& buffer, const VkImage& image);
		void transitionImageLayout(const VkFormat& format, const VkImageLayout& oldLayout, const VkImageLayout& newLayout) const;
	public:
		VulkanTexture() = delete;
		VulkanTexture(const std::string& path, const TextureCreateInfo& createInfo);

		void setUniformBuffer(const void* ubo, const size_t& size);
		void render();
		void release();

		~VulkanTexture();
	};
}