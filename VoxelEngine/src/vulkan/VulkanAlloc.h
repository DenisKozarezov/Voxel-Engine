#pragma once
#include "vulkan/VulkanCommandBuffer.h"

namespace vulkan::memory
{
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

	void createBuffer(const VkPhysicalDevice& physicalDevice, const VkDevice& logicalDevice, const VkDeviceSize& size, const VkBufferUsageFlags& usage, const VkMemoryPropertyFlags& properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
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
		const VkFormat& format);

	const VkImageView createImageView(
		const VkDevice& logicalDevice,
		const VkImage& image,
		const VkFormat& format,
		const VkComponentMapping& components);

	const VkSampler createTextureSampler(
		const VkPhysicalDevice& physicalDevice,
		const VkDevice& logicalDevice);

	void destroyImage(const VkDevice& logicalDevice, const VkImage& image);
	void destroyImageView(const VkDevice& logicalDevice, const VkImageView& imageView);
	void destroyTextureSampler(const VkDevice& logicalDevice, const VkSampler& sampler);
}