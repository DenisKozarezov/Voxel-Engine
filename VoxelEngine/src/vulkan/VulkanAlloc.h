#pragma once
#include <vulkan/vulkan.h>
#include <core/PrimitiveTypes.h>

namespace vulkan::memory
{
	struct Buffer
	{
		VkBuffer buffer;
		VkDeviceMemory bufferMemory;
	};

	const uint32 findMemoryType(const VkPhysicalDevice& physicalDevice, const uint32& typeFilter, const VkMemoryPropertyFlags& properties);
	
	const VkDeviceMemory allocateMemory(const VkPhysicalDevice& physicalDevice, const VkDevice& logicalDevice, const VkMemoryRequirements& requirements, const VkMemoryPropertyFlags& properties);
	
	const VkCommandBuffer& beginSingleTimeCommands();

	void mapMemory(
		const VkDevice& logicalDevice, 
		const VkDeviceMemory& deviceMemory, 
		const VkDeviceSize& offset, 
		const VkDeviceSize& size, 
		const VkMemoryMapFlags& flags, 
		const void* data);

	const Buffer createBuffer(const VkPhysicalDevice& physicalDevice, const VkDevice& logicalDevice, const VkDeviceSize& size, const VkBufferUsageFlags& usage, const VkMemoryPropertyFlags& properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	void destroyBuffer(const VkDevice& logicalDevice, const VkBuffer& buffer);
	void freeDeviceMemory(const VkDevice& logicalDevice, const VkDeviceMemory& memory);

	const VkImage createImage(
		const VkPhysicalDevice& physicalDevice,
		const VkDevice& logicalDevice,
		const uint32& width,
		const uint32& height,
		const VkFormat& format,
		const VkImageTiling& tiling,
		const VkImageUsageFlags& usage,
		const VkMemoryPropertyFlags& properties,
		VkDeviceMemory& imageMemory);

	const VkImageView createImageView(
		const VkDevice& logicalDevice,
		const VkImage& image,
		const VkFormat& format,
		const VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT);

	const VkImageView createImageView(
		const VkDevice& logicalDevice,
		const VkImage& image,
		const VkFormat& format,
		const VkComponentMapping& components,
		const VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT);

	const VkSampler createTextureSampler(
		const VkPhysicalDevice& physicalDevice,
		const VkDevice& logicalDevice);

	void destroyImage(const VkDevice& logicalDevice, const VkImage& image);
	void destroyImageView(const VkDevice& logicalDevice, const VkImageView& imageView);
	void destroyTextureSampler(const VkDevice& logicalDevice, const VkSampler& sampler);
}