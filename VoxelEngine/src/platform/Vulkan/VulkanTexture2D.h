#pragma once
#include <core/renderer/Texture2D.h>
#include <vector>
#include "VulkanVertexBuffer.h"
#include "VulkanIndexBuffer.h"
#include "VulkanUniformBuffer.h"

namespace VoxelEngine::renderer
{
	class VulkanTexture2D : public Texture2D
	{
	private:
		VkDevice _logicalDevice;

		VkImage _textureImage;
		VkImageView _textureImageView;
		VkSampler _textureSampler;
		VkImageLayout _imageLayout;
		VkDeviceMemory _stagingMemory;

		VertexBuffer _vertexBuffer;
		IndexBuffer _indexBuffer;
		UniformBuffer _uniformBuffer;

		VkDescriptorSet _descriptorSet;
		VkDescriptorSetLayout _descriptorSetLayout;
		VkAllocationCallbacks* _allocator;

		std::vector<Vertex> _vertices =
		{
			{ {  1.0f,  1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f } },
			{ { -1.0f,  1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f } },
			{ { -1.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },
			{ {  1.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f } }
		};

		std::vector<uint16> _indices = { 0,1,2,2,3,0 };

		inline const VkDescriptorSet& getDescriptorSet() const & { return _descriptorSet; }
		const VkImageView createImageView(const VkFormat& format) const;
		void generateQuad();
		void createImage(const VkFormat& format, const VkImageTiling& tiling, const VkImageUsageFlags& usage, const VkMemoryPropertyFlags& properties);
		void createTextureImage(const string& filepath);
		void createTextureImageView();
		void createTextureSampler();
		void createDescriptorSet();
		void createDescriptorSetLayout();
		void recordToBuffer(void* src, const VkDeviceSize& imageSize, const VkDeviceMemory& stagingBufferMemory) const;
		void copyBufferToImage(const VkBuffer& buffer, const VkImage& image);
		void transitionImageLayout(const VkFormat& format, const VkImageLayout& oldLayout, const VkImageLayout& newLayout) const;
	public:
		VulkanTexture2D() = delete;
		VulkanTexture2D(const std::string& path, VkAllocationCallbacks* allocator = nullptr);

		void bind();
		void release() override;

		~VulkanTexture2D();
	};
}