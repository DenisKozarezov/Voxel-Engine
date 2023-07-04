#pragma once
#include <core/renderer/Texture.h>
#include <vector>
#include "VulkanVertexBuffer.h"
#include "VulkanIndexBuffer.h"
#include "VulkanUniformBuffer.h"

namespace VoxelEngine::renderer
{
	class VulkanTexture : public Texture
	{
	private:
		VkDevice _logicalDevice;

		VkImage _textureImage;
		VkDeviceMemory _textureImageMemory;
		VkImageView _textureImageView;
		VkSampler _textureSampler;
		VertexBuffer _vertexBuffer;
		IndexBuffer _indexBuffer;
		std::vector<UniformBuffer> _uniformBuffers;
		std::vector<VkDescriptorSet> _descriptorSets;

		const std::vector<Vertex> _vertices = {
			{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
			{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
			{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
		};

		const std::vector<uint16_t> _indices = { 0,1,2,2,3,0 };

		const VkImageView createImageView(const VkFormat& format) const;
		void generateQuad();
		void createImage(const uint32& width, const uint32& height, const VkFormat& format, const VkImageTiling& tiling, const VkImageUsageFlags& usage, const VkMemoryPropertyFlags& properties, VkImage& image, VkDeviceMemory& imageMemory);
		void createTextureImage(const string& filepath);
		void createTextureImageView();
		void createTextureSampler();
		void createDescriptorSets();
		void recordToBuffer(void* src, const VkDeviceSize& imageSize, const VkDeviceMemory& stagingBufferMemory) const;
		void copyBufferToImage(const VkBuffer& buffer, const VkImage& image);
		void transitionImageLayout(const VkFormat& format, const VkImageLayout& oldLayout, const VkImageLayout& newLayout) const;
	public:
		VulkanTexture() = delete;
		VulkanTexture(const std::string& path);

		void updateUniformBuffer(const uint32& currentImage);
		void render(const VkCommandBuffer& commandBuffer, const VkPipelineLayout& pipelineLayout, const uint32& currentImage);
		void release() override;

		~VulkanTexture();
	};
}