#pragma once
#include <vulkan/vulkan.h>
#include <core/renderer/Image.h>

namespace VoxelEngine::renderer
{
	class VulkanImage : public Image
	{
	private:
		VkDevice _logicalDevice;
		VkPhysicalDevice _physicalDevice;

		VkImage _image;
		VkImageView _imageView;
		VkDeviceMemory _memory;
		VkSampler _sampler;

		VkBuffer _stagingBuffer;
		VkDeviceMemory _stagingBufferMemory;
		VkDescriptorSet _descriptorSet;

		inline const VkDescriptorSet& getDescriptorSet() const { return _descriptorSet; }
		const void createImage(const VkFormat& format, const VkImageTiling& tiling, const VkImageUsageFlags& usage, const VkMemoryPropertyFlags& properties);
		const void createImageView(const VkFormat& format, const VkImageAspectFlags& aspectFlags);
		const void createTextureSampler();
		const void createBuffer(const VkDeviceSize& size, const VkBufferUsageFlags& usage, const VkMemoryPropertyFlags& properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) const;
		const void recordToBuffer(void* src, const VkDeviceSize& imageSize) const;
		const void transitionImageLayout(const VkImageLayout& oldLayout, const VkImageLayout& newLayout) const;
		void allocate() override;
		void release() override;
	public:
		void setData(void* data) override;
		void resize(const uint32& width, const uint32& height) override;

		VulkanImage(const std::string_view& path);
		VulkanImage(const uint32& width, const uint32& height, const ImageFormat& format, void* data = nullptr);
		~VulkanImage();
	};
}